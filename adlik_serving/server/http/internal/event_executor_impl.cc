// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>

#include "adlik_serving/server/http/internal/event_executor.h"
#include "cub/task/concurrent_executor.h"

namespace adlik {
namespace serving {

namespace {
struct ExecutorImpl : EventExecutor {
  explicit ExecutorImpl(int num_threads) : executor(std::max(2, num_threads)) {
  }

  void schedule(std::function<void()> fn) override {
    executor.schedule(fn);
  }

private:
  cub::ConcurrentExecutor executor;
};
}  // namespace

std::unique_ptr<EventExecutor> EventExecutor::create(int num_threads) {
  return std::make_unique<ExecutorImpl>(num_threads);
}

}  // namespace serving
}  // namespace adlik
