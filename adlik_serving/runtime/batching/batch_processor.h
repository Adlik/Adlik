#ifndef ADLIK_SERVING_RUNTIME_BATCHING_BATCH_PROCESSOR_H
#define ADLIK_SERVING_RUNTIME_BATCHING_BATCH_PROCESSOR_H

#include <memory>

#include "adlik_serving/runtime/batching/batch_scheduler.h"
#include "adlik_serving/runtime/batching/batching_message_task.h"
#include "cub/base/keywords.h"
#include "tensorflow/core/lib/core/status.h"

namespace adlik {
namespace serving {

struct BatchProcessor {
  virtual ~BatchProcessor() = default;
  virtual uint32_t count() const {
    return 1;
  }
  ABSTRACT(tensorflow::Status processBatch(Batch<BatchingMessageTask>&));
};

}  // namespace serving
}  // namespace adlik

#endif
