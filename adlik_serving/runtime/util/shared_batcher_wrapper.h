// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_UTIL_SHARED_BATCHER_WRAPPER_H
#define ADLIK_SERVING_RUNTIME_UTIL_SHARED_BATCHER_WRAPPER_H

#include <functional>
#include <memory>
#include <string>

#include "adlik_serving/runtime/batching/batch_scheduler.h"
#include "adlik_serving/runtime/batching/batching_message_task.h"
#include "adlik_serving/runtime/batching/shared_batch_scheduler.h"
#include "cub/dci/role.h"

namespace adlik {
namespace serving {

DEFINE_ROLE(SharedBatcherWrapper) {
  using Batcher = SharedBatchScheduler<BatchingMessageTask>;

  using QueueOptions = Batcher::QueueOptions;

  using BatchProcessor = std::function<void(std::unique_ptr<Batch<BatchingMessageTask>> batch)>;
  using Queue = BatchScheduler<BatchingMessageTask>;

  void config(const std::string& name);
  tensorflow::Status append(QueueOptions&, BatchProcessor, std::unique_ptr<Queue>*);

private:
  std::shared_ptr<Batcher> batch_scheduler;
};

}  // namespace serving
}  // namespace adlik

#endif
