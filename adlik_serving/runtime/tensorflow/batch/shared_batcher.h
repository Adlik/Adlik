// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H58D3CB7F_7CD0_4928_9ED7_3187A452F3788
#define H58D3CB7F_7CD0_4928_9ED7_3187A452F3788

#include "adlik_serving/runtime/tensorflow/batch/inferential_task.h"
#include "tensorflow/core/kernels/batching_util/shared_batch_scheduler.h"

namespace tensorflow {

using SharedBatcher = serving::SharedBatchScheduler<InferentialTask>;

}

#endif
