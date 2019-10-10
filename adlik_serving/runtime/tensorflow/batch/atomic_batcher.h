// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H77CA78AC_ECFF_48F2_B456_1EE105FB6089
#define H77CA78AC_ECFF_48F2_B456_1EE105FB6089

#include "adlik_serving/runtime/tensorflow/batch/inferential_task.h"
#include "tensorflow/core/kernels/batching_util/batch_scheduler.h"

namespace tensorflow {

using AtomicBatcher = serving::BatchScheduler<InferentialTask>;
using UniqueBatcher = std::unique_ptr<AtomicBatcher>;

}  // namespace tensorflow

#endif
