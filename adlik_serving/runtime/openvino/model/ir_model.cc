// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/openvino/model/ir_model.h"

#include <stdlib.h>

#include <openvino/openvino.hpp>
#include <unordered_map>

#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/framework/domain/model_id.h"
#include "adlik_serving/runtime/openvino/model/openvino_util.h"
#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "adlik_serving/runtime/provider/predict_response_provider.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/platform/env.h"

OPENVINO_NS_BEGIN

using namespace adlik::serving;

namespace {

#define MY_RETURN_IF_ERROR(...)                         \
  do {                                                  \
    const ::tensorflow::Status _status = (__VA_ARGS__); \
    if (TF_PREDICT_FALSE(!_status.ok()))                \
      return cub::Status(_status.code());               \
  } while (0)

struct PluginLoader : BatchProcessor {
  PluginLoader(const std::string& name, const adlik::serving::ModelConfigProto& config) : name(name), config(config) {
  }

  tensorflow::Status load(const std::shared_ptr<ov::Model>& model, ov::CompiledModel& compiled_model);

  using MyBatch = Batch<BatchingMessageTask>;
  OVERRIDE(tensorflow::Status processBatch(MyBatch&));

private:
  std::string name;
  adlik::serving::ModelConfigProto config;
  mutable ov::InferRequest infer_request;
  std::unordered_map<std::string, ov::Tensor> inputs;
  std::unordered_map<std::string, ov::Tensor> outputs;

  tensorflow::Status mergeInputs(MyBatch&);
  tensorflow::Status splitOutputs(MyBatch&);
};

tensorflow::Status PluginLoader::load(const std::shared_ptr<ov::Model>& model, ov::CompiledModel& compiled_model) {
  // 1. set inferrequest
  infer_request = compiled_model.create_infer_request();

  // 2. get input tensors
  for (auto& input : model->inputs()) {
    ov::Tensor tensor = infer_request.get_tensor(input);
    inputs[input.get_any_name()] = tensor;
  }

  // 3.  get output tensors
  for (auto& output : model->outputs()) {
    ov::Tensor tensor = infer_request.get_tensor(output);
    outputs[output.get_any_name()] = tensor;
  }

  return tensorflow::Status::OK();
}

tensorflow::Status PluginLoader::processBatch(MyBatch& batch) {
  DEBUG_LOG << "Instance \"" << name << "\"running with " << batch.size() << " request payloads";

  auto status = mergeInputs(batch);
  if (!status.ok()) {
    ERR_LOG << "After merge inputs, error message: " << status.error_message() << std::endl;
    return status;
  }
  infer_request.start_async();
  infer_request.wait();
  status = splitOutputs(batch);
  if (!status.ok()) {
    ERR_LOG << "After predict, error message: " << status.error_message();
  }
  return status;
}

tensorflow::Status PluginLoader::mergeInputs(MyBatch& batch) {
  size_t offsetSize = 0;
  tensorflow::Status status = tensorflow::Status::OK();

  for (int i = 0; i < batch.num_tasks(); ++i) {
    const BatchingMessageTask& task = batch.task(i);
    const PredictRequestProvider* requestProvider = task.request;
    const size_t batchSize = requestProvider->batchSize();
    auto func = [&](const std::string& name, const tensorflow::TensorProto& tensor) {
      const void* content = tensor.tensor_content().c_str();
      size_t totalByteSize = tensor.tensor_content().size();

      for (auto& item : inputs) {
        if (name == item.first) {
          ov::Tensor tensor = item.second;
          size_t actualByteSizePerSample = totalByteSize / requestProvider->batchSize();
          size_t offsetByteSize = actualByteSizePerSample * offsetSize;
          size_t copyByteSize = actualByteSizePerSample * batchSize;
          if (offsetByteSize + totalByteSize > tensor.get_byte_size()) {
            status = tensorflow::errors::InvalidArgument("unexpected size ",
                                                         offsetByteSize + totalByteSize,
                                                         " biggger than input blob space, expecting ",
                                                         tensor.get_byte_size());
            return false;
          }
          return copyBuffer2Blob(content, tensor, copyByteSize, offsetByteSize);
        }
      }
      return false;
    };
    requestProvider->visitInputs(func);
    offsetSize += batchSize;
  }
  return status;
}

tensorflow::Status PluginLoader::splitOutputs(MyBatch& batch) {
  for (auto& item : outputs) {
    ov::Tensor tensor = item.second;
    adlik::serving::DimsList dims;
    if (!ConvertDims(tensor.get_shape(), dims)) {
      INFO_LOG << "Invalid blob dims " << item.first << std::endl;
    }
    tensorflow::DataType dtype = ConvertToTensorflowDatatype(tensor.get_element_type());
    size_t offsetByteSize = 0;
    size_t byteSizePerSample = tensor.get_byte_size() / tensor.get_shape()[0];

    for (int i = 0; i < batch.num_tasks(); ++i) {
      const BatchingMessageTask& task = batch.task(i);
      const PredictRequestProvider* requestProvider = task.request;
      PredictResponseProvider* responseProvider = task.response;
      const size_t expectedByteSize = requestProvider->batchSize() * byteSizePerSample;
      void* content = responseProvider->addOutput(item.first, dtype, dims, expectedByteSize);

      if (content == nullptr) {
        // maybe not need this output
      } else {
        if (offsetByteSize + expectedByteSize > tensor.get_byte_size()) {
          return tensorflow::errors::InvalidArgument("unexpected size ",
                                                     offsetByteSize + expectedByteSize,
                                                     " for inference output '",
                                                     item.first,
                                                     "', expecting maximum",
                                                     tensor.get_byte_size());
        } else {
          if (!copyBlob2Buffer(content, tensor, expectedByteSize, offsetByteSize)) {
            return tensorflow::errors::Internal("failed to copy output values from CPU for output '", item.first);
          }
        }
      }
      offsetByteSize += expectedByteSize;
    }
  }
  return tensorflow::Status::OK();
}

tensorflow::Status createInstance(const std::string& instance_name,
                                  const ModelConfig& config,
                                  const std::shared_ptr<ov::Model>& network,
                                  ov::CompiledModel& compiled_model,
                                  std::unique_ptr<BatchProcessor>* model) {
  std::unique_ptr<PluginLoader> raw = std::make_unique<PluginLoader>(instance_name, config);
  TF_RETURN_IF_ERROR(raw->load(network, compiled_model));
  *model = std::move(raw);
  return tensorflow::Status::OK();
}

}  // namespace

IRModel::IRModel(const ModelConfig& config, const ModelId& model_id) : config(config), model_id(model_id) {
}

tensorflow::Status IRModel::init() {
  std::string filePath = config.getModelPath(model_id);
  // 1.read model file
  INFO_LOG << "Read the openvino model";
  std::string xmlFileName = tensorflow::io::JoinPath(filePath, "model.xml");
  // 2.load model
  std::shared_ptr<ov::Model> model;
  ov::Core core;
  try {
    model = core.read_model(xmlFileName);
  } catch (const ov::Exception& e) {
    INFO_LOG << "Cannot load the model";
    return tensorflow::errors::Internal("Cannot load the model ", e.what());
  }
  // 3.set batch size
  model->get_parameters()[0]->set_layout("N...");
  auto currentBatchSize = ov::get_batch(model);
  if (currentBatchSize != config.max_batch_size()) {
    ov::set_batch(model, config.max_batch_size());
  }
  // 4. set instance
  for (const auto& group : config.instance_group()) {
    ov::CompiledModel compiled_model;
    if (group.kind() != adlik::serving::ModelInstanceGroup::KIND_GPU) {
      compiled_model = core.compile_model(model, "CPU");
    } else {
      compiled_model = core.compile_model(model, "GPU");
    }
    for (int i = 0; i != group.count(); ++i) {
      std::unique_ptr<BatchProcessor> instance;
      std::string instance_name = group.name() + "_" + std::to_string(i);
      TF_RETURN_IF_ERROR(createInstance(instance_name, config, model, compiled_model, &instance));
      add(std::move(instance));
    }
  }

  return tensorflow::Status::OK();
}

cub::Status IRModel::create(const ModelConfig& model_config,
                            const ModelId& model_id,
                            std::unique_ptr<IRModel>* model_ptr) {
  ModelConfig copied_config(model_config);
  MY_RETURN_IF_ERROR(NormalizeModelConfig(copied_config));
  MY_RETURN_IF_ERROR(ValidateModelConfig(copied_config));

  model_ptr->reset(new IRModel(copied_config, model_id));
  auto status = (*model_ptr)->init();
  if (!status.ok()) {
    model_ptr->reset();
  }
  INFO_LOG << "After inference engine, status: " << status.error_message();
  return cub::Status(status.code());
}

OPENVINO_NS_END
