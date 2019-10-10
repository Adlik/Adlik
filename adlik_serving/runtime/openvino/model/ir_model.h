// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_OPENVINO_MODEL_IR_MODEL_H
#define ADLIK_SERVING_RUNTIME_OPENVINO_MODEL_IR_MODEL_H

#include <memory>

#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/domain/model_id.h"
#include "adlik_serving/runtime/batching/batching_model.h"
#include "adlik_serving/runtime/batching/composite_batch_processor.h"
#include "adlik_serving/runtime/openvino/openvino.h"
#include "cub/base/status.h"
#include "tensorflow/core/lib/core/status.h"

OPENVINO_NS_BEGIN

struct IRModel : adlik::serving::BatchingModel, adlik::serving::CompositeBatchProcessor {
  static cub::Status create(const adlik::serving::ModelConfig&,
                            const adlik::serving::ModelId&,
                            std::unique_ptr<IRModel>*);

private:
  IRModel(const adlik::serving::ModelConfig& config, const adlik::serving::ModelId& model_id);
  IRModel(IRModel&&) = delete;
  IRModel(const IRModel&) = delete;

  tensorflow::Status init();

  IMPL_ROLE_NS(adlik::serving, BatchProcessor);

  adlik::serving::ModelConfig config;
  adlik::serving::ModelId model_id;
};

OPENVINO_NS_END

#endif
