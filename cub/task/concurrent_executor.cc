// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/task/concurrent_executor.h"

namespace cub {

ConcurrentExecutor::ConcurrentExecutor(int numThreads) : pool(numThreads) {
}

void ConcurrentExecutor::schedule(thread_t f) {
  pool.schedule(std::move(f));
}

}  // namespace cub
