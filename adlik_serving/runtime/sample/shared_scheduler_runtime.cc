// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/batching/batch_processor.h"
#include "adlik_serving/runtime/batching/batching_model.h"
#include "adlik_serving/runtime/util/shared_batcher_runtime_helper.h"

namespace sample {

// A runtime which uses shared batch scheduer to support batching. The Model should
// make sure be thread safe.

namespace {

using namespace adlik::serving;

struct MyModel : BatchProcessor, BatchingModel {
  static cub::Status create(const ModelConfig& config, const ModelId& model_id, std::unique_ptr<MyModel>* result) {
    auto bundle = std::make_unique<MyModel>();

    // todo: do other processes about create model

    *result = std::move(bundle);
    return cub::Success;
  }

private:
  IMPL_ROLE(BatchProcessor);

  using MyBatch = Batch<BatchingMessageTask>;
  tensorflow::Status processBatch(MyBatch&) {
    // todo: process one batch of tasks
    return tensorflow::Status::OK();
  }
};

}  // namespace

DEFINE_SHARED_BATCHER_RUNTIME(SharedScheduler, MyModel);

}  // namespace sample
