// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_FRAMEWORK_BATCHING_SHARED_BATCH_SCHEDULER_H
#define ADLIK_SERVING_FRAMEWORK_BATCHING_SHARED_BATCH_SCHEDULER_H

#include "tensorflow/core/kernels/batching_util/shared_batch_scheduler.h"

namespace adlik {
namespace serving {
template <typename T>
using SharedBatchScheduler = tensorflow::serving::SharedBatchScheduler<T>;
}
}  // namespace adlik

#endif
