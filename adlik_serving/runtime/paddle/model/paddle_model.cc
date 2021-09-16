// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <cstring>
#include <numeric>

#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/runtime/paddle/model/paddle_model.h"
#include "adlik_serving/runtime/paddle/model/paddle_util.h"
#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "adlik_serving/runtime/provider/predict_response_provider.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"
#include "paddle/include/paddle_inference_api.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/platform/env.h"

using namespace adlik::serving;
using paddle::DataType;
using paddle_infer::Config;
using paddle_infer::Predictor;
using paddle_infer::services::PredictorPool;

namespace paddle_runtime {

namespace {

#define MY_RETURN_IF_ERROR(...)                         \
  do {                                                  \
    const ::tensorflow::Status _status = (__VA_ARGS__); \
    if (TF_PREDICT_FALSE(!_status.ok()))                \
      return cub::Status(_status.code());               \
  } while (0)

struct IOBlob {
  IOBlob(std::string name, tensorflow::DataType data_type) : name(name), data_type(data_type) {
  }
  std::string name;
  tensorflow::DataType data_type;
  size_t byte_1batch_size;
  size_t byte_size;
  std::vector<int> shape;
  std::vector<char> buffer;
};

struct Instance : BatchProcessor {
  Instance(const std::string& name, const adlik::serving::ModelConfigProto& config, Predictor* predictor)
      : name(name), config(config), predictor(predictor), max_batch_size(config.max_batch_size()) {
  }
  ~Instance() = default;
  tensorflow::Status init();

private:
  const std::string name;
  const adlik::serving::ModelConfigProto config;
  Predictor* predictor;
  const int max_batch_size;
  std::vector<IOBlob> inputs;
  std::vector<IOBlob> outputs;

  tensorflow::Status processBatch(Batch<BatchingMessageTask>&) override;
  tensorflow::Status mergeInputs(Batch<BatchingMessageTask>&);
  tensorflow::Status splitOutputs(Batch<BatchingMessageTask>&);
};

tensorflow::Status Instance::init() {
  // init inputs
  for (int i = 0; i < config.input_size(); ++i) {
    auto model_input = config.input(i);
    inputs.emplace_back(model_input.name(), model_input.data_type());
    auto& input = inputs[i];
    input.shape.push_back(max_batch_size);
    for (auto dim : model_input.dims())
      input.shape.push_back(dim);
    size_t typesize = GetDataTypeSize(input.data_type);
    if (typesize <= 0)
      return tensorflow::errors::InvalidArgument("unknow data_type ", input.data_type);
    input.byte_1batch_size =
        std::accumulate(++input.shape.begin(), input.shape.end(), 1, std::multiplies<int>()) * typesize;
    input.byte_size = input.byte_1batch_size * max_batch_size;
    input.buffer.resize(input.byte_size);
  }
  // init outputs
  for (int i = 0; i < config.output_size(); ++i) {
    auto model_output = config.output(i);
    outputs.emplace_back(model_output.name(), model_output.data_type());
    auto& output = outputs[i];
    output.shape.push_back(max_batch_size);
    for (auto dim : model_output.dims())
      output.shape.push_back(dim);
    size_t typesize = GetDataTypeSize(output.data_type);
    if (typesize <= 0)
      return tensorflow::errors::InvalidArgument("unknow data_type ", output.data_type);
    output.byte_1batch_size =
        std::accumulate(++output.shape.begin(), output.shape.end(), 1, std::multiplies<int>()) * typesize;
    output.byte_size = output.byte_1batch_size * max_batch_size;
    output.buffer.resize(output.byte_size);
  }
  // check
  auto input_names = predictor->GetInputNames();
  auto output_names = predictor->GetOutputNames();
  if (input_names.size() != inputs.size() || output_names.size() != outputs.size())
    return tensorflow::errors::InvalidArgument("Number of inputs and outputs not match.");
  for (auto& input : inputs) {
    if (std::find(input_names.begin(), input_names.end(), input.name) == input_names.end())
      return tensorflow::errors::InvalidArgument(
          "unexpected input name ", input.name, " in model config. Not match model file.");
  }
  for (auto& output : outputs) {
    if (std::find(output_names.begin(), output_names.end(), output.name) == output_names.end())
      return tensorflow::errors::InvalidArgument(
          "unexpected output name ", output.name, " in model config. Not match model file.");
  }
  return tensorflow::Status::OK();
}

tensorflow::Status Instance::processBatch(Batch<BatchingMessageTask>& payloads) {
  DEBUG_LOG << "Instance \"" << name << "\"running with " << payloads.size() << " request payloads";
  auto status = mergeInputs(payloads);
  if (!status.ok()) {
    ERR_LOG << "After merge inputs, error message: " << status.error_message() << std::endl;
    return status;
  }
  for (auto& input : inputs) {
    auto input_t = predictor->GetInputHandle(input.name);
    input_t->Reshape(input.shape);
    paddle_runtime::CopyFromCpu(std::move(input_t), input.buffer.data(), input.data_type);
  }
  if (!predictor->Run()) {
    ERR_LOG << "Error predict.\n";
    return tensorflow::errors::Internal("Error predict in paddle reference.");
  }
  for (auto& output : outputs) {
    auto output_t = predictor->GetOutputHandle(output.name);
    std::vector<int> output_shape = output_t->shape();
    size_t out_num = std::accumulate(output_shape.begin(), output_shape.end(), 1, std::multiplies<int>());
    if (output.byte_size < out_num)
      return tensorflow::errors::Internal("Get out put size ", out_num, ", bigger than output blob.");
    output.shape[0] = out_num / output.byte_1batch_size;
    paddle_runtime::CopyToCpu(std::move(output_t), output.buffer.data(), output.data_type);
  }
  status = splitOutputs(payloads);
  if (!status.ok()) {
    ERR_LOG << "After predict, error message: " << status.error_message();
  }
  return status;
}

tensorflow::Status Instance::mergeInputs(Batch<BatchingMessageTask>& batch) {
  // flush merged batchinfo to zero
  for (auto& input : inputs) {
    input.shape[0] = 0;
  }
  size_t offsetSize = 0;
  tensorflow::Status status = tensorflow::Status::OK();
  for (int i = 0; i < batch.num_tasks(); ++i) {
    const BatchingMessageTask& task = batch.task(i);
    const PredictRequestProvider* requestProvider = task.request;
    const size_t batchSize = requestProvider->batchSize();
    auto func = [&](const std::string& name, const tensorflow::TensorProto& tensor) {
      const void* content = tensor.tensor_content().c_str();
      size_t totalByteSize = tensor.tensor_content().size();
      for (auto& input : inputs) {
        if (name == input.name) {
          size_t actualByteSizePerSample = totalByteSize / batchSize;
          if (actualByteSizePerSample != input.byte_1batch_size) {
            status = tensorflow::errors::InvalidArgument(
                "unexpected size of 1 batch ", actualByteSizePerSample, ", expecting ", input.byte_1batch_size);
            return false;
          }
          size_t offsetByteSize = actualByteSizePerSample * offsetSize;
          if (offsetByteSize + totalByteSize > input.byte_size) {
            status = tensorflow::errors::InvalidArgument("unexpected size ",
                                                         offsetByteSize + totalByteSize,
                                                         " biggger than input blob space, expecting ",
                                                         input.byte_size);
            return false;
          }
          input.shape[0] = input.shape[0] + batchSize;
          if (std::memcpy(input.buffer.data() + offsetByteSize, content, totalByteSize) !=
              input.buffer.data() + offsetByteSize)
            status = tensorflow::errors::Internal("Fail copy from request to input blob,");
          return true;
        }
        return false;
      }
      return false;
    };
    requestProvider->visitInputs(func);
    offsetSize += batchSize;
  }
  // check input
  for (auto& input : inputs) {
    if (input.shape[0] != offsetSize) {
      ERR_LOG << input.name << " have only " << input.shape[0] << " batch(s) of data, expected " << offsetSize << ".\n";
      return status;
    }
  }
  return status;
}

tensorflow::Status Instance::splitOutputs(Batch<BatchingMessageTask>& batch) {
  for (auto& output : outputs) {
    size_t offsetByteSize = 0;

    for (int i = 0; i < batch.num_tasks(); ++i) {
      adlik::serving::DimsList dims;
      if (!ConvertDims(output.shape, dims)) {
        INFO_LOG << "Invalid blob dims " << output.name << std::endl;
      }
      const BatchingMessageTask& task = batch.task(i);
      const PredictRequestProvider* requestProvider = task.request;
      PredictResponseProvider* responseProvider = task.response;
      const size_t expectedByteSize = requestProvider->batchSize() * output.byte_1batch_size;
      void* content = responseProvider->addOutput(output.name, output.data_type, dims, expectedByteSize);

      if (content == nullptr) {
        // maybe not need this output
      } else {
        if (offsetByteSize + expectedByteSize > output.byte_size) {
          return tensorflow::errors::InvalidArgument("unexpected size ",
                                                     offsetByteSize + expectedByteSize,
                                                     " for inference output '",
                                                     output.name,
                                                     "', expecting maximum",
                                                     output.byte_size);
        } else {
          if (std::memcpy(content, output.buffer.data() + offsetByteSize, expectedByteSize) != content) {
            return tensorflow::errors::Internal("failed to copy output values from CPU for output ", output.name);
          }
        }
      }
      offsetByteSize += expectedByteSize;
    }
  }
  return tensorflow::Status::OK();
}

}  // namespace

PaddleModel::PaddleModel(const ModelConfig& config, const ModelId& model_id) : config(config), model_id(model_id) {
}

tensorflow::Status PaddleModel::init() {
  std::string filePath = config.getModelPath(model_id);
  // 1.read model file
  INFO_LOG << "Read the paddle model";
  std::string modelFileName = tensorflow::io::JoinPath(filePath, "model.pdmodel");
  std::string paramsFileName = tensorflow::io::JoinPath(filePath, "model.pdiparams");
  // 2.setup config
  Config paddle_config;
  paddle_config.SetModel(modelFileName, paramsFileName);
  // TODO: add a config class read options from cmd
  paddle_config.EnableMKLDNN();
  paddle_config.SetCpuMathLibraryNumThreads(1);
  paddle_config.SwitchIrOptim();
  paddle_config.EnableMemoryOptim();
  paddle_config.DisableGlogInfo();
  // 3.create predictor pool
  int instance_count = 0;
  for (const auto& group : config.instance_group()) {
    instance_count += group.count();
  }
  predict_pool = std::make_unique<PredictorPool>(paddle_config, instance_count);
  // 4. set instance
  for (const auto& group : config.instance_group()) {
    for (int i = 0; i != group.count(); ++i) {
      std::string instance_name = group.name() + "_" + std::to_string(i);
      std::unique_ptr<Instance> instance =
          std::make_unique<Instance>(instance_name, config, predict_pool->Retrive(--instance_count));
      TF_RETURN_IF_ERROR(instance->init());
      add(std::move(instance));
    }
  }

  return tensorflow::Status::OK();
}

cub::Status PaddleModel::create(const ModelConfig& model_config,
                                const ModelId& model_id,
                                std::unique_ptr<PaddleModel>* model_ptr) {
  ModelConfig copied_config(model_config);
  MY_RETURN_IF_ERROR(NormalizeModelConfig(copied_config));
  MY_RETURN_IF_ERROR(ValidateModelConfig(copied_config));

  model_ptr->reset(new PaddleModel(copied_config, model_id));
  auto status = (*model_ptr)->init();
  if (!status.ok()) {
    model_ptr->reset();
  }
  INFO_LOG << "After paddle inference, status: " << status.error_message();
  return cub::Status(status.code());
}

}  // namespace paddle_runtime