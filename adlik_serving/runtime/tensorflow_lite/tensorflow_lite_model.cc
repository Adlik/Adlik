// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_model.h"

#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_batch_processor.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

namespace {
using absl::variant;
using adlik::serving::ModelConfig;
using adlik::serving::ModelId;
using adlik::serving::TensorFlowLiteBatchProcessor;
using adlik::serving::TensorFlowLiteModel;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using tensorflow::Status;
using tensorflow::error::Code;
using tflite::FlatBufferModel;
using tflite::Interpreter;
using tflite::ops::builtin::BuiltinOpResolver;

variant<unique_ptr<TensorFlowLiteModel>, Status> internalCreate(const ModelConfig& modelConfig,
                                                                const ModelId& modelId) {
  const auto modelPath = modelConfig.getModelPath(modelId);
  const auto flatBufferModel = shared_ptr<FlatBufferModel>{FlatBufferModel::BuildFromFile(modelPath.c_str())};

  if (!flatBufferModel) {
    return Status{Code::INTERNAL, "Failed to create model"};
  }

  auto result = make_unique<TensorFlowLiteModel>();
  const BuiltinOpResolver opResolver;

  for (const auto& instanceGroup : modelConfig.instance_group()) {
    for (int i = 0; i != instanceGroup.count(); ++i) {
      auto processor = TensorFlowLiteBatchProcessor::create(flatBufferModel, opResolver);

      if (absl::holds_alternative<unique_ptr<TensorFlowLiteBatchProcessor>>(processor)) {
        result->add(absl::get<unique_ptr<TensorFlowLiteBatchProcessor>>(std::move(processor)));
      } else {
        return absl::get<Status>(std::move(processor));
      }
    }
  }

  return std::move(result);
}
}  // namespace

namespace adlik {
namespace serving {
cub::Status TensorFlowLiteModel::create(const ModelConfig& modelConfig,
                                        const ModelId& modelId,
                                        unique_ptr<TensorFlowLiteModel>* model) {
  auto normalizedModelConfig = modelConfig;

  if (NormalizeModelConfig(normalizedModelConfig).ok() && ValidateModelConfig(normalizedModelConfig).ok()) {
    auto result = internalCreate(normalizedModelConfig, modelId);

    if (absl::holds_alternative<unique_ptr<TensorFlowLiteModel>>(result)) {
      *model = absl::get<unique_ptr<TensorFlowLiteModel>>(std::move(result));

      return cub::Success;
    }
  }

  return cub::Failure;
}
}  // namespace serving
}  // namespace adlik
