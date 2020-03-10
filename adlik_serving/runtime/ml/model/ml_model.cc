// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/model/ml_model.h"

#include <mutex>
#include <string>

#include "adlik_serving/apis/task.pb.h"
#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_factory.h"
#include "adlik_serving/runtime/ml/algorithm/ml_task.h"
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
  return;
}

}  // namespace

MLModel::MLModel(const ModelConfig& config, const ModelId& model_id) : config(config), model_id(model_id) {
}

cub::Status MLModel::init() {
  if (config.algorithm().length() == 0) {
    INFO_LOG << "Config algorithm is null, not create algorithm object!";
    return cub::Success;
  }

  if (config.algorithm().length() == 0) {
    ERR_LOG << "Algorithm is null!";
    return cub::InvalidArgument;
  }

  createAlgorithm(config.algorithm(), config.algorithm_config(), &algorithm);
  if (!algorithm) {
    ERR_LOG << "Create ml algorithm failure";
    return cub::Internal;
  }

  return cub::Success;
}

cub::Status MLModel::create(const ModelConfig& config, const ModelId& model_id, std::unique_ptr<MLModel>* bundle) {
  INFO_LOG << "Prepare to create ML model, name: " << model_id.getName() << ", version: " << model_id.getVersion();
  auto raw = std::make_unique<MLModel>(config, model_id);
  auto status = raw->init();
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

  if (!algorithm) {
    ERR_LOG << "Algorithm is null!";
    return cub::StatusWrapper(cub::Internal, "Algorithm is null");
  }

  DEBUG_LOG << "Prepare construct task";

  cub::Notification notification;
  cub::StatusWrapper status;
  auto f = [&]() {
    status = this->algorithm->run(request.task(), *response.mutable_task());
    notification.notify();
  };

  cub::SimpleExecutor executor;
  executor.schedule(f);

  DEBUG_LOG << "Before wait to task";
  notification.wait();

  DEBUG_LOG << "Task is over, status: " << status.error_message();

  if (status.ok()) {
    response.set_task_status("DONE");
  } else {
    response.set_task_status("ERROR");
  }

  return status;
}

}  // namespace ml_runtime
