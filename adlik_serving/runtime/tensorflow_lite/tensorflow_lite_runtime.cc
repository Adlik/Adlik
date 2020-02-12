// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/batching/batching_model.h"
#include "adlik_serving/runtime/batching/composite_batch_processor.h"
#include "adlik_serving/runtime/util/unique_batcher_runtime_helper.h"

namespace {

using adlik::serving::BatchingModel;
using adlik::serving::CompositeBatchProcessor;
using adlik::serving::ModelConfig;
using adlik::serving::ModelId;

class TensorFlowLiteModel : public CompositeBatchProcessor, public BatchingModel {
public:
  static cub::Status create(const ModelConfig& modelConfig,
                            const ModelId& modelId,
                            std::unique_ptr<TensorFlowLiteModel>* model) {
    throw std::logic_error("TODO");
  }
};

DEFINE_UNIQUE_BATCHER_RUNTIME(TensorFlowLite, TensorFlowLiteModel);
}  // namespace
