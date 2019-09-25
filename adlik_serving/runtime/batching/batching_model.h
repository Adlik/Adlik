#ifndef ADLIK_SERVING_RUNTIME_BATCHING_BATCHING_MODEL_H
#define ADLIK_SERVING_RUNTIME_BATCHING_BATCHING_MODEL_H

#include <functional>
#include <memory>

#include "adlik_serving/runtime/batching/batch_scheduler.h"
#include "adlik_serving/runtime/batching/batching_message_task.h"
#include "cub/base/keywords.h"
#include "cub/dci/role.h"
#include "tensorflow/core/lib/core/status.h"

FWD_DECL_STRUCT(tensorflow, RunOptions)

namespace adlik {
namespace serving {

struct PredictRequestProvider;
struct PredictResponseProvider;
struct BatchProcessor;
struct RunOptions;

DEFINE_ROLE(BatchingModel) {
  BatchingModel() = default;

  using SchedulerCreator =
      std::function<tensorflow::Status(std::function<void(std::unique_ptr<Batch<BatchingMessageTask>>)>,
                                       std::unique_ptr<BatchScheduler<BatchingMessageTask>>*)>;

  tensorflow::Status createScheduler(SchedulerCreator creator);

  tensorflow::Status predict(const RunOptions& opts, const PredictRequestProvider*, PredictResponseProvider*);

private:
  TF_DISALLOW_COPY_AND_ASSIGN(BatchingModel);

  USE_ROLE(BatchProcessor);

  std::unique_ptr<BatchScheduler<BatchingMessageTask>> batch_scheduler;
};

}  // namespace serving
}  // namespace adlik

#endif
