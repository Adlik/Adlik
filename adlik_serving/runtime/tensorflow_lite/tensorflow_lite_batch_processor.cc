// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_batch_processor.h"

#include "absl/types/variant.h"
#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/gtl/cleanup.h"

namespace {
using absl::Hash;
using absl::string_view;
using absl::variant;
using adlik::serving::Batch;
using adlik::serving::BatchingMessageTask;
using adlik::serving::PredictRequestProvider;
using adlik::serving::TensorShapeDims;
using std::shared_ptr;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::unordered_map;
using tensorflow::DataType;
using tensorflow::Status;
using tensorflow::TensorProto;
using tensorflow::error::Code;
using tensorflow::gtl::MakeCleanup;
using tflite::Interpreter;

using InputSignature = unordered_map<string_view, tuple<DataType, TensorShapeDims>, Hash<string_view>>;

DataType getTfDataType(TfLiteType type) {
  switch (type) {
    case TfLiteType::kTfLiteNoType:
      return DataType::DT_FLOAT;  //
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

InputSignature getInputSignature(const Interpreter& interpreter) {
  InputSignature result;

  for (const auto i : interpreter.inputs()) {
    const auto& tensor = *interpreter.tensor(i);
    const auto& tfLiteDims = *tensor.dims;

    // TODO: check same first dimension size.

    if (tfLiteDims.size > 0) {
      result.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(tensor.name),
          std::forward_as_tuple(getTfDataType(tensor.type),
                                TensorShapeDims::owned(tfLiteDims.data + 1, tfLiteDims.data + tfLiteDims.size)));
    } else {
      throw std::invalid_argument("Scalar tensors are not supported");
    }
  }

  return result;
}

variant<size_t, Status> checkRequestArguments(InputSignature& argumentSignatureCache,
                                              const PredictRequestProvider& request,
                                              const InputSignature& parameterSignature) {
  constexpr auto invalidBatchSize = static_cast<size_t>(-1);

  const auto cleanup = MakeCleanup([&argumentSignatureCache] { argumentSignatureCache.clear(); });
  auto status = Status::OK();
  size_t batchSize = invalidBatchSize;

  request.visitInputs([&argumentSignatureCache, &batchSize, &status](const string& name, const TensorProto& tensor) {
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
  struct Visitor {
    size_t& totalBatchSize;

    Status operator()(size_t batchSize) {
      this->totalBatchSize += batchSize;

      return Status::OK();
    }

    Status operator()(Status&& status) {
      return status;
    }
  };

  size_t totalBatchSize = 0;
  const auto numTasks = batch.num_tasks();

  for (int i = 0; i != numTasks; ++i) {
    TF_RETURN_IF_ERROR(
        absl::visit(Visitor{totalBatchSize},
                    checkRequestArguments(argumentSignatureCache, *batch.task(i).request, parameterSignature)));
  }

  return totalBatchSize;
}
}  // namespace

namespace adlik {
namespace serving {
TensorFlowLiteBatchProcessor::TensorFlowLiteBatchProcessor(shared_ptr<tflite::FlatBufferModel> model,
                                                           unique_ptr<Interpreter> interpreter)
    : model(std::move(model)),
      interpreter(std::move(interpreter)),
      inputSignature(getInputSignature(*this->interpreter)) {
}

Status TensorFlowLiteBatchProcessor::processBatch(Batch<BatchingMessageTask>& batch) {
  struct Visitor {
    Status operator()(size_t totalBatchSize) {
      throw std::logic_error("Not implemented");
    }

    Status operator()(Status&& status) {
      return status;
    }
  };

  return absl::visit(Visitor{}, checkBatchArguments(this->argumentSignatureCache, batch, this->inputSignature));
}
}  // namespace serving
}  // namespace adlik
