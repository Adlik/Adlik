#ifndef ADLIK_SERVING_FRAMEWORK_BATCHING_BATCH_SCHEDULER_H
#define ADLIK_SERVING_FRAMEWORK_BATCHING_BATCH_SCHEDULER_H

#include "tensorflow/core/kernels/batching_util/batch_scheduler.h"

namespace adlik {
namespace serving {

using BatchTask = tensorflow::serving::BatchTask;

template <typename T>
using Batch = tensorflow::serving::Batch<T>;

template <typename T>
using BatchScheduler = tensorflow::serving::BatchScheduler<T>;

}  // namespace serving
}  // namespace adlik

#endif
