// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_MODEL_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_MODEL_H

#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/runtime/batching/batching_model.h"
#include "adlik_serving/runtime/batching/composite_batch_processor.h"
#include "cub/base/status.h"

namespace adlik {
namespace serving {
struct TensorFlowLiteModel : public CompositeBatchProcessor, public BatchingModel {
  IMPL_ROLE_NS(adlik::serving, BatchProcessor);

  static cub::Status create(const ModelConfig& modelConfig,
                            const ModelId& modelId,
                            std::unique_ptr<TensorFlowLiteModel>* model);
};
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_MODEL_H
