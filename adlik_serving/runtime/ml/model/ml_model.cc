// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/model/ml_model.h"

#include <algorithm>
#include <string>

#include "adlik_serving/apis/task.pb.h"
#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/framework/domain/model_id.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_factory.h"
#include "cub/env/concurrent/notification.h"
#include "cub/log/log.h"
#include "cub/task/simple_executor.h"

namespace ml_runtime {

using namespace adlik::serving;

namespace {

void createAlgorithm(const std::string& name,
                     const adlik::serving::AlgorithmConfig& config,
                     std::unique_ptr<Algorithm>* algorithm) {
  AlgorithmFactory::inst().create(name, config, algorithm);
}

}  // namespace

cub::Status MLModel::init(const ModelConfig& config) {
  if (config.algorithm().length() == 0) {
    INFO_LOG << "Config algorithm is null, not create algorithm object!";
    return cub::Success;
  }

  size_t runner_count = 0;

  for (const auto& group : config.instance_group()) {
    for (int i = 0; i < group.count(); ++i) {
      std::unique_ptr<Algorithm> runner;
      createAlgorithm(config.algorithm(), config.algorithm_config(), &runner);
      if (runner) {
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
  auto status = raw->init(config);
  if (cub::isSuccStatus(status)) {
    *bundle = std::move(raw);
  }

  INFO_LOG << "After Create MLModel, status: " << status;
  return status;
}

cub::StatusWrapper MLModel::run(const CreateTaskRequest& request, CreateTaskResponse& response) {
  if (request.task_type() != CreateTaskRequest_TaskType::CreateTaskRequest_TaskType_TRAINING_TASK) {
    return cub::StatusWrapper(cub::InvalidArgument, "Now only support train task");
  }

  if (!request.is_sync()) {
    return cub::StatusWrapper(cub::InvalidArgument, "Now only suuport synchronous task");
  }

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

  cub::Notification notification;
  cub::StatusWrapper status;
  auto f = [&]() {
    status = it->second->run(request.task(), *response.mutable_task());
    notification.notify();
  };

  cub::SimpleExecutor executor;
  executor.schedule(f);
  notification.wait();

  {
    cub::AutoLock lock(mutex);
    it->first = AVAILABLE;
  }

  DEBUG_LOG << "Task is over, status: " << status.error_message();
  response.set_task_status(status.ok() ? "DONE" : "ERROR");
  return status;
}

}  // namespace ml_runtime
