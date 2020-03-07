// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_batch_processor.h"

#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_engine.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/gtl/cleanup.h"

using absl::Hash;
using absl::string_view;
using absl::variant;
using adlik::serving::Batch;
using adlik::serving::BatchingMessageTask;
using adlik::serving::PredictRequestProvider;
using adlik::serving::TensorShapeDims;
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
using tensorflow::error::Code;
using tensorflow::gtl::MakeCleanup;
using tflite::FlatBufferModel;
using tflite::Interpreter;
using tflite::InterpreterBuilder;
using tflite::OpResolver;

using InputSignature = unordered_map<string_view, tuple<DataType, TensorShapeDims>, Hash<string_view>>;

namespace {
DataType getTfDataType(TfLiteType type) {
  switch (type) {
    case TfLiteType::kTfLiteNoType:
      // https://github.com/tensorflow/tensorflow/blob/4601949937145e66df37483c460ba9b7bfdfa680/tensorflow/lite/delegates/flex/util.cc#L60
      return DataType::DT_FLOAT;
    case TfLiteType::kTfLiteFloat32:
      return DataType::DT_FLOAT;
    case TfLiteType::kTfLiteInt32:
      return DataType::DT_INT32;
    case TfLiteType::kTfLiteUInt8:
      return DataType::DT_UINT8;
    case TfLiteType::kTfLiteInt64:
      return DataType::DT_INT64;
    case TfLiteType::kTfLiteString:
      return DataType::DT_STRING;
    case TfLiteType::kTfLiteBool:
      return DataType::DT_BOOL;
    case TfLiteType::kTfLiteInt16:
      return DataType::DT_INT16;
    case TfLiteType::kTfLiteComplex64:
      return DataType::DT_COMPLEX64;
    case TfLiteType::kTfLiteInt8:
      return DataType::DT_INT8;
    case TfLiteType::kTfLiteFloat16:
      return DataType::DT_HALF;
    default:
      throw std::logic_error("Unreachable");
  }
}

variant<tuple<InputSignature, size_t>, Status> getInputSignature(const Interpreter& interpreter) {
  constexpr auto invalidBatchSize = -1;
  InputSignature result;
  auto batchSize = invalidBatchSize;

  for (const auto i : interpreter.inputs()) {
    const auto& tensor = *interpreter.tensor(i);
    const auto& tfLiteDims = *tensor.dims;

    // TODO: check same first dimension size.

    if (tfLiteDims.size > 0) {
      if (batchSize == invalidBatchSize) {
        batchSize = tfLiteDims.data[0];
      } else if (tfLiteDims.data[0] != batchSize) {
        return Status{Code::INVALID_ARGUMENT, "Inconsistent batch size"};
      }

      result.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(tensor.name),
          std::forward_as_tuple(getTfDataType(tensor.type),
                                TensorShapeDims::owned(tfLiteDims.data + 1, tfLiteDims.data + tfLiteDims.size)));
    } else {
      return Status{Code::INVALID_ARGUMENT, "Scalar tensors are not supported"};
    }
  }

  return make_tuple(std::move(result), batchSize);
}

variant<size_t, Status> checkRequestArguments(InputSignature& argumentSignatureCache,
                                              const PredictRequestProvider& request,
                                              const InputSignature& parameterSignature) {
  constexpr auto invalidBatchSize = static_cast<size_t>(-1);

  const auto cleanup = MakeCleanup([&] { argumentSignatureCache.clear(); });
  auto status = Status::OK();
  size_t batchSize = invalidBatchSize;

  request.visitInputs([&](const string& name, const TensorProto& tensor) {
    const auto& dims = tensor.tensor_shape().dim();

    if (dims.size() > 0) {
      const auto currentBatchSize = static_cast<size_t>(dims[0].size());

      if (batchSize == invalidBatchSize) {
        batchSize = currentBatchSize;
      } else if (currentBatchSize != batchSize) {
        status = Status(Code::INVALID_ARGUMENT, "Inconsistent batch size");

        return false;
      } else {
        argumentSignatureCache.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(name),
            std::forward_as_tuple(tensor.dtype(),
                                  TensorShapeDims::notOwned(dims.data() + 1, dims.data() + dims.size())));

        return true;
      }
    } else {
      status = Status(Code::INVALID_ARGUMENT, "Scalar tensors are not supported");

      return false;
    }
  });

  if (status.ok()) {
    if (argumentSignatureCache == parameterSignature) {
      return batchSize;
    } else {
      status = Status(Code::INVALID_ARGUMENT, "Argument does not match model input signature");
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
    auto result = checkRequestArguments(argumentSignatureCache, *batch.task(i).request, parameterSignature);

    if (absl::holds_alternative<size_t>(result)) {
      totalBatchSize += absl::get<size_t>(std::move(result));
    } else {
      return absl::get<Status>(std::move(result));
    }
  }

  return totalBatchSize;
}

Status updateInterpreterBatchSize(Interpreter& interpreter, size_t batchSize, vector<int>& dimsCache) {
  const auto cleanup1 = MakeCleanup([&] { dimsCache.clear(); });

  dimsCache.push_back(static_cast<int>(batchSize));

  for (const auto i : interpreter.inputs()) {
    const auto& dims = *interpreter.tensor(i)->dims;

    dimsCache.resize(static_cast<size_t>(dims.size));

    std::copy(dims.data + 1, dims.data + dims.size, dimsCache.begin() + 1);

    if (interpreter.ResizeInputTensor(i, dimsCache) != TfLiteStatus::kTfLiteOk) {
      return Status{Code::INTERNAL, "Unable to resize input tensor"};
    }
  }

  if (interpreter.AllocateTensors() != TfLiteStatus::kTfLiteOk) {
    return Status{Code::INTERNAL, "Unable to allocate tensors"};
  }

  return Status::OK();
}
}  // namespace

namespace adlik {
namespace serving {
TensorFlowLiteBatchProcessor::TensorFlowLiteBatchProcessor(ConstructCredential,
                                                           shared_ptr<tflite::FlatBufferModel> model,
                                                           unique_ptr<Interpreter> interpreter,
                                                           InputSignature parameterSignature,
                                                           size_t lastBatchSize)
    : model(std::move(model)),
      interpreter(std::move(interpreter)),
      parameterSignature(std::move(parameterSignature)),
      lastBatchSize(lastBatchSize) {
}

Status TensorFlowLiteBatchProcessor::processBatch(Batch<BatchingMessageTask>& batch) {
  auto result = checkBatchArguments(this->argumentSignatureCache, batch, this->parameterSignature);

  if (absl::holds_alternative<Status>(result)) {
    return absl::get<Status>(std::move(result));
  }

  const auto batchSize = absl::get<0>(result);

  if (batchSize != this->lastBatchSize) {
    auto updateBatchSizeResult = updateInterpreterBatchSize(*this->interpreter, batchSize, this->inputTensorDimsCache);

    if (!updateBatchSizeResult.ok()) {
      return std::move(updateBatchSizeResult);
    }

    this->lastBatchSize = batchSize;
  }

  return processTensorFlowLiteTask(*this->interpreter, batch);
}

variant<unique_ptr<TensorFlowLiteBatchProcessor>, Status> TensorFlowLiteBatchProcessor::create(
    shared_ptr<FlatBufferModel> model,
    const OpResolver& opResolver) {
  unique_ptr<Interpreter> interpreter;

  if (InterpreterBuilder{*model, opResolver}(&interpreter, 1) != TfLiteStatus::kTfLiteOk) {
    return Status(Code::INTERNAL, "Unable to create interpreter");
  }

  if (interpreter->AllocateTensors() != TfLiteStatus::kTfLiteOk) {
    return Status(Code::INTERNAL, "Unable to allocate tensors");
  }

  auto maybeSignatureAndBatchSize = getInputSignature(*interpreter);

  if (absl::holds_alternative<tuple<InputSignature, size_t>>(maybeSignatureAndBatchSize)) {
    auto signatureAndBatchSize = absl::get<tuple<InputSignature, size_t>>(std::move(maybeSignatureAndBatchSize));

    return make_unique<TensorFlowLiteBatchProcessor>(constructCredential,
                                                     std::move(model),
                                                     std::move(interpreter),
                                                     std::move(std::get<0>(signatureAndBatchSize)),
                                                     std::move(std::get<1>(signatureAndBatchSize)));
  } else {
    return absl::get<1>(std::move(maybeSignatureAndBatchSize));
  }
}
}  // namespace serving
}  // namespace adlik
