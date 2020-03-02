// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/batching/composite_batch_processor.h"

#include "cub/log/log.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

void CompositeBatchProcessor::add(std::unique_ptr<BatchProcessor> instance) {
  tensorflow::mutex_lock lock(mu);
  processors.push_back(std::move(instance));
}

uint32_t CompositeBatchProcessor::count() const {
  return processors.size();
}

tensorflow::Status CompositeBatchProcessor::processBatch(Batch<BatchingMessageTask>& batch) {
  std::unique_ptr<BatchProcessor> processor;

  {
    tensorflow::mutex_lock lock(mu);

    if (processors.empty()) {
      return tensorflow::errors::Internal(
          "Can't find an available instance, should check whether number of threads match instances!");
    }

    processor = std::move(processors.back());
    processors.pop_back();
  }

  auto status = processor->processBatch(batch);

  {
    tensorflow::mutex_lock lock(mu);
    processors.push_back(std::move(processor));
  }

  return status;
}

}  // namespace serving
}  // namespace adlik
