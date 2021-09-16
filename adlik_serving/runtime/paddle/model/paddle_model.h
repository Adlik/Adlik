// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_PADDLE_MODEL_PADDLE_MODEL_H
#define ADLIK_SERVING_RUNTIME_PADDLE_MODEL_PADDLE_MODEL_H

#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/domain/model_id.h"
#include "adlik_serving/runtime/batching/batching_model.h"
#include "adlik_serving/runtime/batching/composite_batch_processor.h"
#include "cub/base/status.h"
#include "paddle/include/paddle_inference_api.h"
#include "tensorflow/core/lib/core/status.h"

namespace paddle_runtime {

struct PaddleModel : adlik::serving::BatchingModel, adlik::serving::CompositeBatchProcessor {
  static cub::Status create(const adlik::serving::ModelConfig&,
                            const adlik::serving::ModelId&,
                            std::unique_ptr<PaddleModel>*);

private:
  PaddleModel(const adlik::serving::ModelConfig& config, const adlik::serving::ModelId& model_id);
  PaddleModel(PaddleModel&&) = delete;
  PaddleModel(const PaddleModel&) = delete;

  tensorflow::Status init();

  IMPL_ROLE_NS(adlik::serving, BatchProcessor);

  adlik::serving::ModelConfig config;
  adlik::serving::ModelId model_id;
  std::unique_ptr<paddle_infer::services::PredictorPool> predict_pool;
};

}  // namespace paddle_runtime
#endif
