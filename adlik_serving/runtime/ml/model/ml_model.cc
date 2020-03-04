// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/model/ml_model.h"

#include <mutex>
#include <string>

#include "adlik_serving/apis/task.pb.h"
#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_config.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_factory.h"
#include "adlik_serving/runtime/ml/algorithm/ml_task.h"
#include "cub/env/concurrent/notification.h"
#include "cub/log/log.h"
#include "cub/task/simple_executor.h"

namespace ml_runtime {

using namespace adlik::serving;

namespace {

void create_algorithm(std::unique_ptr<Algorithm>* algorithm) {
  AlgorithmConfig algorithm_config;
  AlgorithmFactory::inst().create("k-means", algorithm_config, algorithm);
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

  // just temporaryl
  if (config.algorithm() != "k-means") {
    ERR_LOG << "Unsupported algorithm '" << config.algorithm() << "', now only support k-means!";
    return cub::InvalidArgument;
  }

  create_algorithm(&algorithm);
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

cub::StatusWrapper MLModel::run(const CreateTaskRequest& request, CreateTaskResponse&) {
  if (request.task_type() != CreateTaskRequest_TaskType::CreateTaskRequest_TaskType_TRAINING_TASK) {
    return cub::StatusWrapper(cub::InvalidArgument, "Now only support train task");
  }

  if (!request.is_sync()) {
    return cub::StatusWrapper(cub::InvalidArgument, "Now only suuport synchronous task");
  }

  if (request.algorithm().length() > 0) {
    if (request.algorithm() != "k-means") {
      return cub::StatusWrapper(cub::InvalidArgument, "Now only suuport k-means task");
    }
    if (!request.has_kmeans_task()) {
      return cub::StatusWrapper(cub::InvalidArgument, "Not have k-means task parameters");
    }
    if (!algorithm) {
      create_algorithm(&algorithm);
    }
  }

  if (!algorithm) {
    ERR_LOG << "Algorithm is null!";
    return cub::StatusWrapper(cub::Internal, "Algorithm is null");
  }

  DEBUG_LOG << "Prepare construct task";

  MLTask ml_task;
  auto& task = ml_task.kmeans;
  task.input = request.kmeans_task().input();
  task.n_clusters = request.kmeans_task().n_clusters();
  task.max_iter = request.kmeans_task().max_iter();
  task.compute_labels = request.kmeans_task().compute_labels();
  task.label_name = request.kmeans_task().label_name();
  task.output = request.kmeans_task().output();

  cub::Notification notification;
  auto f = [&]() {
    this->algorithm->run(ml_task);
    notification.notify();
  };

  cub::SimpleExecutor executor;
  executor.schedule(f);

  DEBUG_LOG << "Before wait to task";
  notification.wait();

  DEBUG_LOG << "Task is over, status: " << ml_task.status.error_message();

  // TODO: do something for exception
  return ml_task.status;
}

}  // namespace ml_runtime
