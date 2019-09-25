#ifndef ADLIK_SERVING_RUNTIME_BATCHING_BATCHING_MESSAGE_TASK_H
#define ADLIK_SERVING_RUNTIME_BATCHING_BATCHING_MESSAGE_TASK_H

#include "adlik_serving/runtime/batching/batch_scheduler.h"
#include "cub/base/fwddecl.h"

FWD_DECL_STRUCT(tensorflow, Status)
FWD_DECL_STRUCT(tensorflow, Notification)

namespace adlik {
namespace serving {

struct PredictRequestProvider;
struct PredictResponseProvider;

struct BatchingMessageTask : BatchTask {
  size_t size() const override {
    return zeroth_dim_size;
  }

  // Fields populated when a task is received.
  uint64_t enqueue_time_micros;
  // RunOptions run_options;
  size_t zeroth_dim_size;

  const PredictRequestProvider* request;
  PredictResponseProvider* response;

  // Fields populated when a task is processed (as part of a batch).
  tensorflow::Notification* done;
  tensorflow::Status* status;
  uint64_t timeout_in_ms = 0;
  //   RunMetadata* run_metadata;
};

}  // namespace serving
}  // namespace adlik

#endif
