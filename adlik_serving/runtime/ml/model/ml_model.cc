// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/model/ml_model.h"

#include <algorithm>
#include <string>

#include "adlik_serving/apis/task.pb.h"
#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/domain/model_id.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_factory.h"
#include "cub/env/concurrent/notification.h"
#include "cub/log/log.h"
#include "cub/task/simple_executor.h"

namespace ml_runtime {

using namespace adlik::serving;

cub::Status MLModel::init(const ModelConfig& config, const ModelId& model_id) {
  if (config.algorithm().length() == 0) {
    INFO_LOG << "Config algorithm is null, not create algorithm object!";
    return cub::Success;
  }

  size_t runner_count = 0;
  auto model_path = config.getModelPath(model_id);
  for (const auto& group : config.instance_group()) {
    for (int i = 0; i < group.count(); ++i) {
      std::unique_ptr<Algorithm> runner;
      auto status = AlgorithmFactory::inst().create(config.algorithm(), model_path, &runner);
      if (status.ok() && runner) {
        runners.push_back({AVAILABLE, std::move(runner)});
        DEBUG_LOG << "Create instance " << runner_count << " for model " << config.getModelName();
      } else {
        ERR_LOG << "Create ml algorithm failure";
        return cub::Internal;
      }
      runner_count++;
    }
  }

  if (runner_count == 0) {
    ERR_LOG << "Runner count is 0 when create model " << config.getModelName();
    return cub::InvalidArgument;
  }

  return cub::Success;
}

cub::Status MLModel::create(const ModelConfig& config, const ModelId& model_id, std::unique_ptr<MLModel>* bundle) {
  INFO_LOG << "Prepare to create ML model, name: " << model_id.getName() << ", version: " << model_id.getVersion();
  auto raw = std::make_unique<MLModel>();
  auto status = raw->init(config, model_id);
  if (cub::isSuccStatus(status)) {
    *bundle = std::move(raw);
  }

  INFO_LOG << "After Create MLModel, status: " << status;
  return status;
}

cub::StatusWrapper MLModel::run(const CreateTaskRequest& request, CreateTaskResponse& response) {
  auto it = runners.begin();
  {
    cub::AutoLock lock(mutex);
    it = std::find_if(runners.begin(), runners.end(), [](const auto& item) { return item.first == AVAILABLE; });
    if (it == runners.end()) {
      return cub::StatusWrapper(
          cub::Unavailable,
          "Can't find an available instance, should check whether number of threads match instances!");
    } else {
      it->first = UNAVAILABLE;
    }
  }

  cub::StatusWrapper status;
  cub::Notification notification;
  std::atomic<bool> is_timeout(false);
  auto should_terminate = [&]() { return is_timeout.load(); };

  auto f = [&]() {
    status = it->second->run(request.detail(), *response.mutable_detail(), should_terminate);
    notification.notify();
  };

  cub::SimpleExecutor executor;
  executor.schedule(f);
  auto timeout_ms = request.timeout_seconds() == 0 ? ULONG_MAX : request.timeout_seconds() * 1000;
  auto notified = notification.wait(timeout_ms);
  if (!notified) {
    is_timeout = true;
  }

  notification.wait();

  {
    cub::AutoLock lock(mutex);
    it->first = AVAILABLE;
  }

  DEBUG_LOG << "Task is over, status: " << status.error_message();
  response.set_status(status.ok() ? "DONE" : "ERROR");
  return status;
}

}  // namespace ml_runtime
