// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/batching/batching_model.h"

#include "adlik_serving/framework/manager/run_options.h"
#include "adlik_serving/runtime/batching/batch_processor.h"
#include "adlik_serving/runtime/batching/cleanup.h"
#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "cub/env/time/time.h"
#include "cub/log/log.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

namespace {
struct Processor {
  Processor(BatchProcessor& impl) : impl(impl) {
  }

  void operator()(std::unique_ptr<Batch<BatchingMessageTask>> batch) {
    batch->WaitUntilClosed();

    if (batch->empty()) {
      return;
    }

    tensorflow::Status status;
    auto finally = MakeCleanup([&status, &batch] {
      for (int i = 0; i < batch->num_tasks(); ++i) {
        *batch->mutable_task(i)->status = status;
        batch->mutable_task(i)->done->Notify();
      }
    });

    bool all_tasks_timeout_exceeded = true;
    const uint64_t dequeue_time_micros = cub::timesystem().nowMicros();
    uint64_t batch_deadline_micros = 0;
    for (int i = 0; i < batch->num_tasks(); ++i) {
      const BatchingMessageTask& task = batch->task(i);
      const int64_t task_timeout_micros = task.timeout_in_ms <= 0 ? INT_MAX : task.timeout_in_ms * 1000;
      const uint64_t task_deadline_micros = task.enqueue_time_micros + task_timeout_micros;
      if (task_deadline_micros > dequeue_time_micros) {
        all_tasks_timeout_exceeded = false;
        if (task_deadline_micros > batch_deadline_micros) {
          batch_deadline_micros = task_deadline_micros;
        }
      }
    }
    if (all_tasks_timeout_exceeded) {
      status = tensorflow::Status(tensorflow::error::RESOURCE_EXHAUSTED,
                                  "Run() timeout exceeded while waiting in batching queue");
      return;
    }
    status = impl.processBatch(*batch);
  }

private:
  BatchProcessor& impl;
};

}  // namespace

tensorflow::Status BatchingModel::createScheduler(SchedulerCreator creator) {
  if (batch_scheduler) {
    return tensorflow::errors::Internal("Attempt to change scheduler not allowed");
  }
  return creator(
      [this](std::unique_ptr<Batch<BatchingMessageTask>> batch) {
        Processor(this->ROLE(BatchProcessor))(std::move(batch));
      },
      &batch_scheduler);
}

tensorflow::Status BatchingModel::predict(const RunOptions& opts,
                                          const PredictRequestProvider* request,
                                          PredictResponseProvider* response) {
  if (!batch_scheduler) {
    ERR_LOG << "Batch scheduler is null, can't do btach inference";
    return tensorflow::errors::Internal("Batch scheduler is null, can't do btach inference");
  }

  auto task = std::make_unique<BatchingMessageTask>();
  tensorflow::Notification done;
  tensorflow::Status status;
  task->enqueue_time_micros = cub::timesystem().nowMicros();
  task->request = request;
  task->response = response;
  task->zeroth_dim_size = request->batchSize();
  task->done = &done;
  task->status = &status;
  task->timeout_in_ms = opts.timeout_in_ms;
  TF_RETURN_IF_ERROR(batch_scheduler->Schedule(&task));
  done.WaitForNotification();
  return status;
}

}  // namespace serving
}  // namespace adlik
