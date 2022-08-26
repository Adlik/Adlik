// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#if MEMORY_CUDA
#include <c10/cuda/CUDACachingAllocator.h>
#include <cuda_runtime_api.h>
#endif

#include <torch/cuda.h>
#include <torch/script.h>
#include <torchvision/vision.h>

#include <algorithm>
#include <cstring>
#include <iostream>

#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/runtime/libtorch/model/libtorch_model.h"
#include "adlik_serving/runtime/libtorch/model/libtorch_util.h"
#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "adlik_serving/runtime/provider/predict_response_provider.h"
#include "adlik_serving/runtime/util/datatype_size.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/platform/env.h"

using namespace adlik::serving;
namespace libtorch_runtime {

namespace {

#define MY_RETURN_IF_ERROR(...)                         \
  do {                                                  \
    const ::tensorflow::Status _status = (__VA_ARGS__); \
    if (TF_PREDICT_FALSE(!_status.ok()))                \
      return cub::Status(_status.code());               \
  } while (0)

#define MY_RETURN_IF_ERROR_TF(...)                      \
  do {                                                  \
    const ::tensorflow::Status _status = (__VA_ARGS__); \
    if (TF_PREDICT_FALSE(!_status.ok()))                \
      return _status;                                   \
  } while (0)

c10::List<at::Tensor> TupleToList(const c10::intrusive_ptr<c10::ivalue::Tuple>& torch_tuple) {
  std::vector<c10::IValue> torch_vec = torch_tuple->elements();
  c10::List<at::Tensor> result;
  for (auto& value : torch_vec) {
    if (value.isTensor())
      result.push_back(value.toTensor());
    else if (value.isTensorList()) {
      auto torch_list = value.toTensorList();
      for (at::Tensor tensor : torch_list)
        result.push_back(tensor);
    }
  }
  return result;
}

struct IOBlob {
  IOBlob(std::string name, tensorflow::DataType data_type)
      : name(std::move(name)), data_type(data_type), byte_1batch_size(0), byte_size(0) {
  }
  std::string name;
  tensorflow::DataType data_type;
  size_t byte_1batch_size;
  size_t byte_size;
  std::vector<int> shape;
  std::vector<char> buffer;
};

struct Instance : BatchProcessor {
  Instance(const std::string& name,
           int gpu_id,
           const adlik::serving::ModelConfigProto& config,
           torch::jit::script::Module&& module)
      : name(name), gpu_id(gpu_id), config(config), max_batch_size(config.max_batch_size()), torch_module(module) {
  }
  ~Instance() {
    INFO_LOG << "~Instance()";
#if MEMORY_CUDA
    c10::cuda::CUDACachingAllocator::emptyCache();
#endif
  }
  tensorflow::Status init();

private:
  const std::string name;
  int gpu_id;
  const adlik::serving::ModelConfigProto config;
  const int max_batch_size;
  torch::jit::script::Module torch_module;
  std::vector<torch::jit::IValue> torch_inputs;
  std::vector<IOBlob> inputs;
  std::vector<IOBlob> outputs;
  tensorflow::Status processBatch(Batch<BatchingMessageTask>&) override;
  tensorflow::Status mergeInputs(Batch<BatchingMessageTask>&);
  tensorflow::Status splitOutputs(Batch<BatchingMessageTask>&);
};

tensorflow::Status Instance::init() {
  auto init_blob = [&](IOBlob& blob, const adlik::serving::DimsList& dims) {
    blob.shape.push_back(max_batch_size);
    for (auto dim : dims)
      blob.shape.push_back(dim);
    size_t typesize = GetDataTypeSize(blob.data_type);
    if (typesize <= 0)
      return tensorflow::errors::InvalidArgument("unknow data_type ", blob.data_type);
    blob.byte_1batch_size =
        std::accumulate(++blob.shape.begin(), blob.shape.end(), 1, std::multiplies<int>()) * typesize;
    blob.byte_size = blob.byte_1batch_size * max_batch_size;
    blob.buffer.resize(blob.byte_size);
    return tensorflow::Status::OK();
  };
  // init inputs
  for (int i = 0; i < config.input_size(); ++i) {
    auto model_input = config.input(i);
    inputs.emplace_back(model_input.name(), model_input.data_type());
    MY_RETURN_IF_ERROR_TF(init_blob(inputs.back(), model_input.dims()));
  }
  // init outputs
  for (int i = 0; i < config.output_size(); ++i) {
    auto model_output = config.output(i);
    outputs.emplace_back(model_output.name(), model_output.data_type());
    MY_RETURN_IF_ERROR_TF(init_blob(outputs.back(), model_output.dims()));
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

  torch::Tensor tensor_in;
  for (auto& input : inputs) {
    std::vector<int64_t> shape;
    DEBUG_LOG << "libtorch max_bath_size: "
              << " [" << config.max_batch_size() << "]";
    for (int i = 0; i < input.shape.size(); i++)
      shape.emplace_back(input.shape[i]);
    torch::Dtype torch_data_type = libtorch_runtime::ConvertDatatype(input.data_type);
    tensor_in = torch::from_blob(input.buffer.data(), at::IntArrayRef(shape)).toType(torch_data_type);
    torch::Device device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU, this->gpu_id);
    torch_inputs.emplace_back(tensor_in.to(device));
  }
  torch::jit::IValue torch_outputs;
  // auto torch_outputs = torch_module.forward(torch_inputs);
  try {
    torch_outputs = torch_module.forward(torch_inputs);
  } catch (const std::exception& e) {
    ERR_LOG << "Cannot forward the model: " << e.what();
    return tensorflow::errors::Internal("Forward the model fail.");
  }

  torch_inputs.clear();
  size_t offsetSize = 0;

  if (outputs.size() > 1) {
    int i = 0;
    auto tpl = torch_outputs.toTuple();
    auto torch_list = TupleToList(tpl);
    for (auto& output : outputs) {
      at::Tensor torch_output = torch_list[i];
      // memcpy can't used on cuda
      torch_output = torch_output.to(torch::kCPU);
      size_t out_byte_num = torch_output.nbytes();
      if (output.byte_size < out_byte_num)
        return tensorflow::errors::Internal("Get out put size ", out_byte_num, ", bigger than output blob.");
      output.shape[0] = out_byte_num / output.byte_1batch_size;
      void* ptr = torch_output.data_ptr();
      std::memcpy(output.buffer.data(), ptr, out_byte_num);
      offsetSize += out_byte_num;
      i = i + 1;
    }
  } else {
    for (auto& output : outputs) {
      auto torch_output = torch_outputs.toTensor();
      torch_output = torch_output.to(torch::kCPU);
      size_t out_byte_num = torch_output.nbytes();
      if (output.byte_size < out_byte_num)
        return tensorflow::errors::Internal("Get out put size ", out_byte_num, ", bigger than output blob.");
      output.shape[0] = out_byte_num / output.byte_1batch_size;
      void* ptr = torch_output.data_ptr();
      std::memcpy(output.buffer.data(), ptr, out_byte_num);
      offsetSize += out_byte_num;
    }
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
LibtorchModel::LibtorchModel(const adlik::serving::ModelConfig& config, const adlik::serving::ModelId& model_id)
    : config(config), model_id(model_id) {
}

tensorflow::Status LibtorchModel::init() {
  std::string filePath = config.getModelPath(model_id);
  // 1.read model file
  INFO_LOG << "Read the torch model";
  std::string modelFileName = tensorflow::io::JoinPath(filePath, "model.pt");
  for (const auto& group : config.instance_group()) {
    for (int i = 0; i != group.count(); ++i) {
      // config.instance_group() is const struct
      std::vector<int> gpu_ids;
      if (group.gpus().size() == 0)
        gpu_ids.emplace_back(0);
      else
        for (auto num_id : group.gpus())
          gpu_ids.emplace_back(num_id);
      for (int gpu_device : gpu_ids) {
        std::string instance_name = group.name() + "_" + std::to_string(gpu_device) + "_" + std::to_string(i);
        torch::jit::script::Module module;
        torch::Device device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU, gpu_device);
        try {
          module = torch::jit::load(modelFileName, device);
        } catch (const c10::Error& e) {
          ERR_LOG << "Cannot load the model";
          return tensorflow::errors::Internal("Cannot load the model: ", e.what());
        }
        INFO_LOG << "Created instance " << instance_name << " on GPU " << gpu_device;
        std::unique_ptr<Instance> instance =
            std::make_unique<Instance>(instance_name, gpu_device, config, std::move(module));
        TF_RETURN_IF_ERROR(instance->init());
        add(std::move(instance));
      }
    }
  }

  return tensorflow::Status::OK();
}

cub::Status LibtorchModel::create(const ModelConfig& model_config,
                                  const ModelId& model_id,
                                  std::unique_ptr<LibtorchModel>* model_ptr) {
  ModelConfig copied_config(model_config);
  NormalizeModelConfig(copied_config);
  MY_RETURN_IF_ERROR(ValidateModelConfig(copied_config));

  model_ptr->reset(new LibtorchModel(copied_config, model_id));
  auto status = (*model_ptr)->init();
  if (!status.ok()) {
    model_ptr->reset();
  }
  INFO_LOG << "After torch inference, status: " << status.error_message();
  return cub::Status(status.code());
}

}  // namespace libtorch_runtime
