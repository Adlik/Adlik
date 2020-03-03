// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/task/simple_executor.h"
#include "cub/env/concurrent/concurrent.h"

namespace cub {

void SimpleExecutor::schedule(thread_t f) {
  concurrent().start(f);
}

}  // namespace cub
