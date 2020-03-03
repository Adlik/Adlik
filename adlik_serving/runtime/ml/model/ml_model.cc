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
#include "tensorflow/core/lib/core/errors.h"

namespace ml_runtime {

using namespace adlik::serving;

namespace {

tensorflow::Status create_algorithm(std::unique_ptr<Algorithm>* algorithm) {
  AlgorithmConfig algorithm_config;
  AlgorithmFactory::inst().create("k-means", algorithm_config, algorithm);
  if (!algorithm) {
    ERR_LOG << "Create ml algorithm failure";
    return tensorflow::errors::Internal("Create ml algorithm failure!");
  }

  return tensorflow::Status::OK();
}

}  // namespace

MLModel::MLModel(const ModelConfig& config, const ModelId& model_id) : config(config), model_id(model_id) {
}

tensorflow::Status MLModel::init() {
  if (config.algorithm().length() == 0) {
    INFO_LOG << "Config algorithm is null, not create algorithm object!";
    return tensorflow::errors::InvalidArgument("Config algorithm is null, not create algorithm object!");
  }

  // just temporaryl
  if (config.algorithm() != "k-means") {
    return tensorflow::errors::Internal("Unsupported algorithm '", config.algorithm(), "', now only support k-means!");
  }

  AlgorithmConfig algorithm_config;
  AlgorithmFactory::inst().create(config.algorithm(), algorithm_config, &algorithm);
  if (!algorithm) {
    ERR_LOG << "Create ml algorithm failure";
    return tensorflow::errors::Internal("Create ml algorithm failure!");
  }

  return tensorflow::Status::OK();
}

cub::Status MLModel::create(const ModelConfig& config, const ModelId& model_id, std::unique_ptr<MLModel>* bundle) {
  INFO_LOG << "Prepare to create ML model, name: " << model_id.getName() << ", version: " << model_id.getVersion();
  auto raw = std::make_unique<MLModel>(config, model_id);
  auto status = raw->init();
  if (status.ok()) {
    *bundle = std::move(raw);
  }

  INFO_LOG << "After Create MLModel, status: " << status.error_message();
  return cub::Status(status.code());
}

tensorflow::Status MLModel::run(const CreateTaskRequest& request, CreateTaskResponse&) {
  if (request.task_type() != CreateTaskRequest_TaskType::CreateTaskRequest_TaskType_TRAINING_TASK) {
    return tensorflow::errors::InvalidArgument("Now only support train task");
  }

  if (!request.is_sync()) {
    return tensorflow::errors::InvalidArgument("Now only suuport synchronous task");
  }

  if (request.algorithm().length() > 0) {
    if (request.algorithm() != "k-means") {
      return tensorflow::errors::InvalidArgument("Now only suuport k-means task");
    }
    if (!create_algorithm(&algorithm).ok())
      return tensorflow::errors::Internal("Create algorithm failure");
  }

  if (!request.has_kmeans_task()) {
    return tensorflow::errors::InvalidArgument("Not have k-means task parameters");
  }

  MLTask ml_task;
  auto& task = ml_task.kmeans;
  task.input = request.kmeans_task().input();
  task.n_clusters = request.kmeans_task().n_clusters();
  task.max_iter = request.kmeans_task().max_iter();
  task.compute_labels = request.kmeans_task().compute_labels();
  task.lable_name = request.kmeans_task().lable_name();
  task.output = request.kmeans_task().output();

  cub::Notification notification;
  auto f = [&]() {
    this->algorithm->run(ml_task);
    notification.notify();
  };

  cub::SimpleExecutor executor;
  executor.schedule(f);

  notification.wait();

  // TODO: do something for exception
  return tensorflow::Status::OK();
}

}  // namespace ml_runtime
