// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/batching/batching_model.h"
#include "adlik_serving/runtime/batching/composite_batch_processor.h"
#include "adlik_serving/runtime/util/unique_batcher_runtime_helper.h"

namespace {

using adlik::serving::Batch;
using adlik::serving::BatchingMessageTask;
using adlik::serving::BatchingModel;
using adlik::serving::BatchProcessor;
using adlik::serving::CompositeBatchProcessor;
using adlik::serving::ModelConfig;
using adlik::serving::ModelId;

class TensorFlowLiteBatchProcessor : public BatchProcessor {
  virtual tensorflow::Status processBatch(Batch<BatchingMessageTask>& batch) override {
    throw std::logic_error("TODO");
  }
};

class TensorFlowLiteModel : public CompositeBatchProcessor, public BatchingModel {
public:
  IMPL_ROLE_NS(adlik::serving, BatchProcessor);

  static cub::Status create(const ModelConfig& modelConfig,
                            const ModelId& modelId,
                            std::unique_ptr<TensorFlowLiteModel>* model) {
    auto tempModel = std::make_unique<TensorFlowLiteModel>();

    for (const auto& _ : modelConfig.instance_group()) {
      tempModel->add(std::make_unique<TensorFlowLiteBatchProcessor>());
    }

    *model = std::move(tempModel);

    return cub::Success;
  }
};

DEFINE_UNIQUE_BATCHER_RUNTIME(TensorFlowLite, TensorFlowLiteModel);

}  // namespace
