// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/model/scheduler.h"

#include <functional>

#include "adlik_serving/runtime/batching/basic_batch_scheduler.h"
#include "adlik_serving/runtime/batching/batch_scheduler.h"
#include "adlik_serving/runtime/ml/algorithm/ml_task.h"

namespace ml_runtime {

using namespace adlik::serving;

using TaskProcessor = std::function<void(std::unique_ptr<Batch<MLTask>> task)>;

void processTask(std::unique_ptr<Batch<MLTask>>) {
}

void createScheduler(int treads_count) {
  using Batcher = BasicBatchScheduler<MLTask>;
  std::unique_ptr<Batcher> raw_scheduler;

  Batcher::Options options;
  options.num_batch_threads = treads_count;
  options.thread_pool_name = "_batch_threads";
  options.max_batch_size = treads_count;  // todo

  auto status = Batcher::Create(options, processTask, &raw_scheduler);
  if (status.ok()) {
    // todo;
  }
}
}  // namespace ml_runtime
