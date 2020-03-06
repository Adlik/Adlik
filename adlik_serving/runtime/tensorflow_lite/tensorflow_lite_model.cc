// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_model.h"

#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_batch_processor.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

namespace {
using adlik::serving::ModelConfig;
using adlik::serving::ModelId;
using adlik::serving::TensorFlowLiteBatchProcessor;
using adlik::serving::TensorFlowLiteModel;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using tflite::FlatBufferModel;
using tflite::Interpreter;
using tflite::InterpreterBuilder;
using tflite::ops::builtin::BuiltinOpResolver;

unique_ptr<TensorFlowLiteModel> internalCreate(const ModelConfig& modelConfig, const ModelId& modelId) {
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
}  // namespace

namespace adlik {
namespace serving {
cub::Status TensorFlowLiteModel::create(const ModelConfig& modelConfig,
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
}  // namespace serving
}  // namespace adlik
