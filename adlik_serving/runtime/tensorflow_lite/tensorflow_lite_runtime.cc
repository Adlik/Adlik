// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "absl/hash/hash.h"
#include "absl/types/variant.h"
#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/runtime/batching/batching_model.h"
#include "adlik_serving/runtime/batching/composite_batch_processor.h"
#include "adlik_serving/runtime/tensorflow_lite/tensor_shape_dims.h"
#include "adlik_serving/runtime/util/unique_batcher_runtime_helper.h"
#include "tensorflow/core/lib/gtl/cleanup.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

namespace {

using absl::string_view;
using absl::variant;
using adlik::serving::Batch;
using adlik::serving::BatchingMessageTask;
using adlik::serving::BatchingModel;
using adlik::serving::BatchProcessor;
using adlik::serving::CompositeBatchProcessor;
using adlik::serving::ModelConfig;
using adlik::serving::ModelId;
using adlik::serving::NormalizeModelConfig;
using adlik::serving::PredictRequestProvider;
using adlik::serving::PredictResponseProvider;
using adlik::serving::TensorShapeDims;
using adlik::serving::ValidateModelConfig;
using std::make_unique;
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
using tflite::FlatBufferModel;
using tflite::Interpreter;
using tflite::InterpreterBuilder;
using tflite::ops::builtin::BuiltinOpResolver;

class TensorFlowLiteBatchProcessor : public BatchProcessor {
  using InputSignature = unordered_map<string_view, tuple<DataType, TensorShapeDims>, absl::Hash<string_view>>;

  const shared_ptr<FlatBufferModel> model;  // Make sure the model is alive when interpreter is alive.
  unique_ptr<Interpreter> interpreter;
  const InputSignature inputSignature;
  mutable InputSignature argumentSignatureCache;

  static DataType getTfDataType(TfLiteType type) {
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

  static InputSignature getInputSignature(const Interpreter& interpreter) {
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

  variant<size_t, Status> checkRequestArguments(const PredictRequestProvider& request) const {
    constexpr auto invalidBatchSize = static_cast<size_t>(-1);

    auto& argumentSignature = this->argumentSignatureCache;
    const auto cleanup = MakeCleanup([&argumentSignature] { argumentSignature.clear(); });
    auto status = Status::OK();
    size_t batchSize = invalidBatchSize;

    request.visitInputs([&argumentSignature, &batchSize, &status](const string& name, const TensorProto& tensor) {
      const auto& dims = tensor.tensor_shape().dim();

      if (dims.size() > 0) {
        const auto currentBatchSize = static_cast<size_t>(dims[0].size());

        if (batchSize == invalidBatchSize) {
          batchSize = currentBatchSize;
        } else if (currentBatchSize != batchSize) {
          status = Status(Code::INVALID_ARGUMENT, "Inconsistent batch size");

          return false;
        } else {
          argumentSignature.emplace(
              std::piecewise_construct,
              std::forward_as_tuple(name),
              std::forward_as_tuple(tensor.dtype(),
                                    TensorShapeDims::nonOwned(dims.data() + 1, dims.data() + dims.size())));

          return true;
        }
      } else {
        status = Status(Code::INVALID_ARGUMENT, "Scalar tensors are not supported");

        return false;
      }
    });

    if (status.ok()) {
      if (this->argumentSignatureCache == this->inputSignature)
        return batchSize;
    } else {
      status = Status(Code::INVALID_ARGUMENT, "Argument does not match model input signature");
    }

    return status;
  }

  variant<size_t, Status> checkBatchArguments(Batch<BatchingMessageTask>& batch) {
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
      TF_RETURN_IF_ERROR(absl::visit(Visitor{totalBatchSize}, this->checkRequestArguments(*batch.task(i).request)));
    }

    return totalBatchSize;
  }

  virtual Status processBatch(Batch<BatchingMessageTask>& batch) override {
    struct Visitor {
      Status operator()(size_t totalBatchSize) {
        throw std::logic_error("Not implemented");
      }

      Status operator()(Status&& status) {
        return status;
      }
    };

    return absl::visit(Visitor{}, this->checkBatchArguments(batch));
  }

public:
  TensorFlowLiteBatchProcessor(shared_ptr<FlatBufferModel> model, unique_ptr<Interpreter> interpreter)
      : model(std::move(model)),
        interpreter(std::move(interpreter)),
        inputSignature(getInputSignature(*this->interpreter)) {
  }
};  // namespace

class TensorFlowLiteModel : public CompositeBatchProcessor, public BatchingModel {
  static unique_ptr<TensorFlowLiteModel> internalCreate(const ModelConfig& modelConfig, const ModelId& modelId) {
    const auto modelPath = modelConfig.getModelPath(modelId);
    const auto flatBufferModel = shared_ptr<FlatBufferModel>(FlatBufferModel::BuildFromFile(modelPath.c_str()));

    if (!flatBufferModel) {
      return nullptr;
    }

    const BuiltinOpResolver opResolver;
    InterpreterBuilder interpreterBuilder{*flatBufferModel, opResolver};
    auto result = make_unique<TensorFlowLiteModel>();

    for (const auto& instanceGroup : modelConfig.instance_group()) {
      for (int i = 0; i != instanceGroup.count(); ++i) {
        unique_ptr<Interpreter> interpreter;

        if (interpreterBuilder(&interpreter, 1) == TfLiteStatus::kTfLiteOk) {
          result->add(make_unique<TensorFlowLiteBatchProcessor>(flatBufferModel, std::move(interpreter)));
        } else {
          return nullptr;
        }
      }
    }

    return result;
  }

public:
  IMPL_ROLE_NS(adlik::serving, BatchProcessor);

  static cub::Status create(const ModelConfig& modelConfig,
                            const ModelId& modelId,
                            unique_ptr<TensorFlowLiteModel>* model) {
    auto normalizedModelConfig = modelConfig;

    if (!NormalizeModelConfig(normalizedModelConfig).ok() || !ValidateModelConfig(normalizedModelConfig).ok()) {
      return cub::Failure;
    };

    auto result = internalCreate(normalizedModelConfig, modelId);

    if (result) {
      *model = std::move(result);

      return cub::Success;
    } else {
      return cub::Failure;
    }
  }
};

DEFINE_UNIQUE_BATCHER_RUNTIME(TensorFlowLite, TensorFlowLiteModel);

}  // namespace
