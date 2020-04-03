// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_model.h"

#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_batch_processor.h"
#include "cub/log/log.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/io/path.h"
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
using tensorflow::errors::Internal;
using tensorflow::io::JoinPath;
using tflite::FlatBufferModel;
using tflite::ops::builtin::BuiltinOpResolver;

variant<unique_ptr<TensorFlowLiteModel>, Status> internalCreate(const ModelConfig& modelConfig,
                                                                const ModelId& modelId) {
  auto normalizedModelConfig = modelConfig;

  TF_RETURN_IF_ERROR(NormalizeModelConfig(normalizedModelConfig));
  TF_RETURN_IF_ERROR(ValidateModelConfig(normalizedModelConfig));

  const auto modelPath = JoinPath(normalizedModelConfig.getModelPath(modelId), "model.tflite");
  const auto flatBufferModel = shared_ptr<FlatBufferModel>{FlatBufferModel::BuildFromFile(modelPath.c_str())};

  if (!flatBufferModel) {
    return Internal("Failed to create model");
  }

  auto result = make_unique<TensorFlowLiteModel>();
  const BuiltinOpResolver opResolver;

  for (const auto& instanceGroup : normalizedModelConfig.instance_group()) {
    for (int i = 0; i != instanceGroup.count(); ++i) {
      auto processor = TensorFlowLiteBatchProcessor::create(flatBufferModel, opResolver);

      if (absl::holds_alternative<unique_ptr<TensorFlowLiteBatchProcessor>>(processor)) {
        result->add(std::move(absl::get<unique_ptr<TensorFlowLiteBatchProcessor>>(processor)));
      } else {
        return std::move(absl::get<Status>(processor));
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
  auto result = internalCreate(modelConfig, modelId);

  if (absl::holds_alternative<unique_ptr<TensorFlowLiteModel>>(result)) {
    *model = std::move(absl::get<unique_ptr<TensorFlowLiteModel>>(result));

    return cub::Success;
  } else {
    ERR_LOG << absl::get<Status>(result) << std::endl;

    return cub::Failure;
  }
}
}  // namespace serving
}  // namespace adlik
