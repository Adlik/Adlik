// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/batching/composite_batch_processor.h"

#include "cub/log/log.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

void CompositeBatchProcessor::add(std::unique_ptr<BatchProcessor> instance) {
  tensorflow::mutex_lock lock(mu);
  processors.push_back({AVAILABLE, std::move(instance)});
}

uint32_t CompositeBatchProcessor::count() const {
  return processors.size();
}

tensorflow::Status CompositeBatchProcessor::processBatch(Batch<BatchingMessageTask>& batch) {
  auto it = processors.begin();
  {
    tensorflow::mutex_lock lock(mu);
    it = std::find_if(processors.begin(), processors.end(), [](const auto& item) { return item.first == AVAILABLE; });

    if (it == processors.end()) {
      return tensorflow::errors::Internal(
          "Can't find an available instance, should check whether number of threads "
          "match instances!");
    } else {
      // Make model unavailable
      it->first = UNAVAILABLE;
    }
  }

  auto status = it->second->processBatch(batch);
  {
    // Make model available
    tensorflow::mutex_lock lock(mu);
    it->first = AVAILABLE;
  }
  return status;
}

}  // namespace serving
}  // namespace adlik
