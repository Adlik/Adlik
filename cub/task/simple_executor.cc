// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/task/simple_executor.h"

namespace cub {

void SimpleExecutor::schedule(thread_t f) {
  f();
}

}  // namespace cub
