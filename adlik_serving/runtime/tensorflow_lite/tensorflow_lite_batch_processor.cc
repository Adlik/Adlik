// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_batch_processor.h"

#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"
#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_engine.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/gtl/cleanup.h"

using absl::Hash;
using absl::MakeSpan;
using absl::string_view;
using absl::variant;
using adlik::serving::Batch;
using adlik::serving::BatchingMessageTask;
using adlik::serving::getTFDataTypeName;
using adlik::serving::InputContext;
using adlik::serving::ModelConfigProto;
using adlik::serving::ModelInput;
using adlik::serving::ModelOutput;
using adlik::serving::OutputContext;
using adlik::serving::PredictRequestProvider;
using adlik::serving::TensorShapeDims;
using adlik::serving::tfLiteTypeToTfType;
using google::protobuf::RepeatedPtrField;
using std::make_unique;
using std::shared_ptr;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::unordered_map;
using std::vector;
using tensorflow::DataType;
using tensorflow::Status;
using tensorflow::TensorProto;
using tensorflow::TensorShapeProto_Dim;
using tensorflow::errors::Internal;
using tensorflow::errors::InvalidArgument;
using tensorflow::gtl::MakeCleanup;
using tflite::FlatBufferModel;
using tflite::Interpreter;
using tflite::InterpreterBuilder;
using tflite::OpResolver;

template <class T>
using StringViewMap = unordered_map<string_view, T, Hash<string_view>>;
using InputSignature = StringViewMap<tuple<DataType, TensorShapeDims>>;

namespace {
string displaySignature(const InputSignature& signature) {
  std::stringstream stream;

  auto writeList = [&](const auto& container, const char* separator, auto itemWriter) {
    auto it = container.begin();
    auto last = container.end();

    if (it != last) {
      itemWriter(*it);

      for (++it; it != last; ++it) {
        stream << separator;

        itemWriter(*it);
      }
    }
  };

  stream << '(';

  writeList(signature, ", ", [&](const InputSignature::value_type& arg) {
    stream << arg.first << ": " << getTFDataTypeName(std::get<0>(arg.second)) << '[';
    writeList(std::get<1>(arg.second), ", ", [&](const TensorShapeProto_Dim* dim) { stream << dim->size(); });
    stream << ']';
  });

  stream << ')';

  return stream.str();
}

variant<tuple<InputSignature, size_t>, Status> getSignature(const Interpreter& interpreter,
                                                            const vector<int>& tensorIndices) {
  constexpr auto invalidBatchSize = -1;
  InputSignature result;
  auto batchSize = invalidBatchSize;

  for (const auto i : tensorIndices) {
    const auto& tensor = *interpreter.tensor(i);
    const auto& tfLiteDims = *tensor.dims;

    // TODO: check same first dimension size.

    if (tfLiteDims.size > 0) {
      if (batchSize == invalidBatchSize) {
        batchSize = tfLiteDims.data[0];
      } else if (tfLiteDims.data[0] != batchSize) {
        return InvalidArgument("Inconsistent batch size");
      }

      result.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(tensor.name),
          std::forward_as_tuple(tfLiteTypeToTfType(tensor.type),
                                TensorShapeDims::owned(tfLiteDims.data + 1, tfLiteDims.data + tfLiteDims.size)));
    } else {
      return InvalidArgument("Scalar tensors are not supported");
    }
  }

  return make_tuple(std::move(result), batchSize);
}

template <class T>
Status checkSignature(const InputSignature& modelSignature, const RepeatedPtrField<T>& declaredSignature) {
  InputSignature signature2;

  for (const auto& item : declaredSignature) {
    signature2.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(item.name()),
        std::forward_as_tuple(item.data_type(), TensorShapeDims::owned(item.dims().begin(), item.dims().end())));
  }

  if (modelSignature == signature2) {
    return Status::OK();
  } else {
    return Internal("Model signature does not match declared one. Model signature: ",
                    displaySignature(modelSignature),
                    ". Declared signature: ",
                    displaySignature(modelSignature),
                    ".");
  }
}

StringViewMap<InputContext> getInputContextMap(const Interpreter& interpreter) {
  StringViewMap<InputContext> result;

  for (const auto i : interpreter.inputs()) {
    const auto tensor = *interpreter.tensor(i);

    result.emplace(std::piecewise_construct,
                   std::forward_as_tuple(tensor.name),
                   std::forward_as_tuple(InputContext::create(i, tensor.type)));
  }

  return result;
}

vector<OutputContext> getOutputContexts(const Interpreter& interpreter) {
  vector<OutputContext> result;

  for (const auto i : interpreter.outputs()) {
    result.push_back(OutputContext::fromTfLiteTensor(i, *interpreter.tensor(i)));
  }

  return result;
}

Status checkRequestArguments(InputSignature& argumentSignatureCache,
                             const PredictRequestProvider& request,
                             const InputSignature& parameterSignature) {
  const auto cleanup = MakeCleanup([&] { argumentSignatureCache.clear(); });
  auto status = Status::OK();

  request.visitInputs([&](const string& name, const TensorProto& tensor) {
    const auto& dims = tensor.tensor_shape().dim();

    if (dims.size() > 0) {
      const auto batchSize = static_cast<size_t>(dims[0].size());

      if (batchSize != request.batchSize()) {
        status = InvalidArgument("Inconsistent batch size");

        return false;
      } else {
        argumentSignatureCache.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(name),
            std::forward_as_tuple(tensor.dtype(),
                                  TensorShapeDims::borrowed(dims.data() + 1, dims.data() + dims.size())));

        return true;
      }
    } else {
      status = InvalidArgument("Scalar tensors are not supported");

      return false;
    }
  });

  if (status.ok()) {
    if (argumentSignatureCache != parameterSignature) {
      status = InvalidArgument("Argument does not match model input signature. Input signature: ",
                               displaySignature(parameterSignature),
                               ". Argument signature: ",
                               displaySignature(argumentSignatureCache),
                               '.');
    }
  }

  return status;
}

variant<size_t, Status> checkBatchArguments(InputSignature& argumentSignatureCache,
                                            Batch<BatchingMessageTask>& batch,
                                            const InputSignature& parameterSignature) {
  size_t totalBatchSize = 0;
  const auto numTasks = batch.num_tasks();

  for (int i = 0; i != numTasks; ++i) {
    const auto& request = *batch.task(i).request;

    TF_RETURN_IF_ERROR(checkRequestArguments(argumentSignatureCache, request, parameterSignature));

    totalBatchSize += request.batchSize();
  }

  return totalBatchSize;
}

Status updateInterpreterBatchSize(Interpreter& interpreter, size_t batchSize, vector<int>& dimsCache) {
  const auto cleanup = MakeCleanup([&] { dimsCache.clear(); });

  dimsCache.push_back(static_cast<int>(batchSize));

  for (const auto i : interpreter.inputs()) {
    const auto& dims = *interpreter.tensor(i)->dims;

    dimsCache.resize(static_cast<size_t>(dims.size));

    std::copy(dims.data + 1, dims.data + dims.size, dimsCache.begin() + 1);

    if (interpreter.ResizeInputTensor(i, dimsCache) != TfLiteStatus::kTfLiteOk) {
      return Internal("Unable to resize input tensor");
    }
  }

  if (interpreter.AllocateTensors() != TfLiteStatus::kTfLiteOk) {
    return Internal("Unable to allocate tensors");
  }

  return Status::OK();
}
}  // namespace

namespace adlik {
namespace serving {
TensorFlowLiteBatchProcessor::TensorFlowLiteBatchProcessor(ConstructCredential,
                                                           shared_ptr<FlatBufferModel> model,
                                                           unique_ptr<Interpreter> interpreter,
                                                           InputSignature parameterSignature,
                                                           size_t lastBatchSize,
                                                           StringViewMap<InputContext> inputContextMap,
                                                           vector<OutputContext> outputContexts)
    : model(std::move(model)),
      interpreter(std::move(interpreter)),
      parameterSignature(std::move(parameterSignature)),
      lastBatchSize(lastBatchSize),
      inputContextMap(std::move(inputContextMap)),
      outputContexts(std::move(outputContexts)) {
}

Status TensorFlowLiteBatchProcessor::processBatch(Batch<BatchingMessageTask>& batch) {
  auto result = checkBatchArguments(this->argumentSignatureCache, batch, this->parameterSignature);

  if (absl::holds_alternative<Status>(result)) {
    return std::move(absl::get<Status>(result));
  }

  const auto batchSize = absl::get<size_t>(result);

  if (batchSize != this->lastBatchSize) {
    auto updateBatchSizeResult = updateInterpreterBatchSize(*this->interpreter, batchSize, this->inputTensorDimsCache);

    if (!updateBatchSizeResult.ok()) {
      return updateBatchSizeResult;
    }

    this->lastBatchSize = batchSize;
  }

  return processTensorFlowLiteTask(*this->interpreter, this->inputContextMap, MakeSpan(this->outputContexts), batch);
}

variant<unique_ptr<TensorFlowLiteBatchProcessor>, Status> TensorFlowLiteBatchProcessor::create(
    shared_ptr<FlatBufferModel> model,
    const OpResolver& opResolver,
    const ModelConfigProto& modelConfigProto) {
  unique_ptr<Interpreter> interpreter;

  if (InterpreterBuilder{*model, opResolver}(&interpreter, 1) != TfLiteStatus::kTfLiteOk) {
    return Internal("Unable to create interpreter");
  }

  if (interpreter->AllocateTensors() != TfLiteStatus::kTfLiteOk) {
    return Internal("Unable to allocate tensors");
  }

  // Check input signature.

  auto maybeInputSignature = getSignature(*interpreter, interpreter->inputs());

  if (!absl::holds_alternative<tuple<InputSignature, size_t>>(maybeInputSignature)) {
    return std::move(absl::get<Status>(maybeInputSignature));
  }

  auto inputSignature = std::move(absl::get<tuple<InputSignature, size_t>>(maybeInputSignature));

  TF_RETURN_IF_ERROR(checkSignature(std::get<InputSignature>(inputSignature), modelConfigProto.input()));

  // Check output signature.

  auto maybeOutputSignature = getSignature(*interpreter, interpreter->outputs());

  if (!absl::holds_alternative<tuple<InputSignature, size_t>>(maybeOutputSignature)) {
    return std::move(absl::get<Status>(maybeOutputSignature));
  }

  TF_RETURN_IF_ERROR(
      checkSignature(std::get<InputSignature>(absl::get<tuple<InputSignature, size_t>>(maybeOutputSignature)),
                     modelConfigProto.output()));

  // Get IO contexts.

  auto inputContextMap = getInputContextMap(*interpreter);
  auto outputContexts = getOutputContexts(*interpreter);

  return make_unique<TensorFlowLiteBatchProcessor>(ConstructCredential{},
                                                   std::move(model),
                                                   std::move(interpreter),
                                                   std::move(std::get<InputSignature>(inputSignature)),
                                                   std::move(std::get<size_t>(inputSignature)),
                                                   std::move(inputContextMap),
                                                   std::move(outputContexts));
}
}  // namespace serving
}  // namespace adlik
