// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_BATCHING_COMPOSITE_BATCH_PROCESSOR_H
#define ADLIK_SERVING_RUNTIME_BATCHING_COMPOSITE_BATCH_PROCESSOR_H

#include <memory>
#include <utility>
#include <vector>

#include "adlik_serving/runtime/batching/batch_processor.h"
#include "cub/base/keywords.h"
#include "tensorflow/core/platform/macros.h"
#include "tensorflow/core/platform/mutex.h"
#include "tensorflow/core/platform/thread_annotations.h"

namespace adlik {
namespace serving {

struct CompositeBatchProcessor : BatchProcessor {
  void add(std::unique_ptr<BatchProcessor>);

  uint32_t count() const;

private:
  OVERRIDE(tensorflow::Status processBatch(Batch<BatchingMessageTask>&));

  enum State { AVAILABLE = 0, UNAVAILABLE = 1 };

  tensorflow::mutex mu;
  std::vector<std::unique_ptr<BatchProcessor>> processors GUARDED_BY(mu);
};

}  // namespace serving
}  // namespace adlik

#endif
