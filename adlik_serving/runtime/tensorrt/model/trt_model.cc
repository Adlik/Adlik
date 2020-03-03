// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorrt/model/trt_model.h"

#include <cuda_runtime_api.h>

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/runtime/batching/basic_batch_scheduler.h"
#include "adlik_serving/runtime/batching/batch_scheduler.h"
#include "adlik_serving/runtime/batching/batching_message_task.h"
#include "adlik_serving/runtime/tensorrt/model/trt_instance.h"
#include "adlik_serving/runtime/tensorrt/model/trt_util.h"
#include "cub/log/log.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/platform/env.h"

namespace tensorrt {

using namespace adlik::serving;

namespace {

std::string nameOfPlan(const ModelConfigProto& config, const int gpu_device) {
  cudaDeviceProp cuprops;
  cudaError_t cuerr = cudaGetDeviceProperties(&cuprops, gpu_device);
  if (cuerr != cudaSuccess) {
    FATAL_LOG << "unable to get CUDA device properties for " << config.name() << ": " << cudaGetErrorString(cuerr);
  }
  return kTensorRTPlanFilename;
}

tensorflow::Status createInstance(const ModelConfigProto& config,
                                  const std::string& instance_name,
                                  const int gpu_device,
                                  const std::unordered_map<std::string, std::vector<char>>& models,
                                  std::unique_ptr<BatchProcessor>* context) {
  const std::string cc_model_filename = nameOfPlan(config, gpu_device);
  const auto& mn_itr = models.find(cc_model_filename);
  if (mn_itr == models.end()) {
    return tensorflow::errors::Internal("unable to find PLAN model '", cc_model_filename, "' for ", config.name());
  }

  TF_RETURN_IF_ERROR(CreateTrtInstance(config, instance_name, gpu_device, mn_itr->second, context));

  INFO_LOG << "Created instance " << instance_name << " on GPU " << gpu_device << " (" << cc_model_filename << ")";
  return tensorflow::Status::OK();
}

tensorflow::Status readAllFiles(const std::string& dir_name,
                                std::unordered_map<std::string, std::vector<char>>& contents) {
  std::vector<std::string> possible_children;
  TF_RETURN_IF_ERROR(tensorflow::Env::Default()->GetChildren(dir_name, &possible_children));

  std::set<std::string> children;
  for (const auto& child : possible_children) {
    children.insert(child.substr(0, child.find_first_of('/')));
  }

  for (const auto& filename : children) {
    const auto plan_path = tensorflow::io::JoinPath(dir_name, filename);
    tensorflow::string model_data_str;
    TF_RETURN_IF_ERROR(tensorflow::ReadFileToString(tensorflow::Env::Default(), plan_path, &model_data_str));
    contents.emplace(std::piecewise_construct,
                     std::forward_as_tuple(filename),
                     std::forward_as_tuple(model_data_str.begin(), model_data_str.end()));
  }
  return tensorflow::Status::OK();
}

}  // namespace

TrtModel::TrtModel(const ModelConfig& config, const ModelId& model_id) : config(config), model_id(model_id) {
}

tensorflow::Status TrtModel::init() {
  std::unordered_map<std::string, std::vector<char>> models;
  TF_RETURN_IF_ERROR(readAllFiles(config.getModelPath(model_id), models));

  // serialized with a global lock.
  static std::mutex global_context_mu;
  std::lock_guard<std::mutex> glock(global_context_mu);
  uint32_t total_context_count = 0;
  for (const auto& group : config.instance_group()) {
    // TensorRT requires that every context have a GPU.
    if ((group.kind() != adlik::serving::ModelInstanceGroup::KIND_GPU) || (group.gpus().size() == 0)) {
      return tensorflow::errors::InvalidArgument("instance group ",
                                                 group.name(),
                                                 " of model ",
                                                 config.name(),
                                                 " must be KIND_GPU and must specify at least on GPU id");
    }

    for (int c = 0; c < group.count(); c++) {
      for (int gpu_device : group.gpus()) {
        const std::string instance_name = group.name() + "_" + std::to_string(c) + "_gpu" + std::to_string(gpu_device);
        std::unique_ptr<BatchProcessor> instance;
        TF_RETURN_IF_ERROR(createInstance(config, instance_name, gpu_device, models, &instance));
        add(std::move(instance));
        total_context_count++;
      }
    }
  }
  return tensorflow::Status::OK();
}

cub::Status TrtModel::create(const ModelConfig& config, const ModelId& model_id, std::unique_ptr<TrtModel>* bundle) {
  adlik::serving::ModelConfig copied_config(config);
  auto status = NormalizeModelConfig(copied_config);
  if (!status.ok()) {
    return cub::Status(status.code());
  }

  auto raw = std::unique_ptr<TrtModel>(new TrtModel(copied_config, model_id));

  status = raw->init();
  if (status.ok()) {
    *bundle = std::move(raw);
  }

  INFO_LOG << "After CreateTrtModel, status: " << status.error_message();
  return cub::Status(status.code());
}

}  // namespace tensorrt
