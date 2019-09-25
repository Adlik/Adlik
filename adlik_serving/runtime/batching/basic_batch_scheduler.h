#ifndef ADLIK_SERVING_RUNTIME_BATCHING_BASIC_BATCH_SCHEDULER_H
#define ADLIK_SERVING_RUNTIME_BATCHING_BASIC_BATCH_SCHEDULER_H

#include "tensorflow/core/kernels/batching_util/basic_batch_scheduler.h"

namespace adlik {
namespace serving {

template <typename T>
using BasicBatchScheduler = tensorflow::serving::BasicBatchScheduler<T>;
}
}  // namespace adlik

#endif
