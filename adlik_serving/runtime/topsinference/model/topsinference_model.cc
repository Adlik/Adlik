// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/topsinference/model/topsinference_model.h"

#include <algorithm>
#include <cstring>
#include <numeric>

#include <TopsInference/TopsInferRuntime.h>
#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "adlik_serving/runtime/provider/predict_response_provider.h"
#include "adlik_serving/runtime/topsinference/model/topsinference_util.h"
#include "adlik_serving/runtime/util/datatype_size.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/errors.h"

namespace topsinference_runtime {

using namespace adlik::serving;
using namespace TopsInference;

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

struct DeviceSetter {
  DeviceSetter() : handler(nullptr) {
    TopsInference::topsInference_init();
    constexpr int card_id = 0;
    constexpr uint32_t cluster_id[] = {0, 1, 2, 3, 4, 5};
    auto* handler = TopsInference::set_device(card_id, cluster_id, 6, nullptr);
  }
  void func() {
    INFO_LOG << "do set_device";
  }
  ~DeviceSetter() {
    if (handler != nullptr) {
      TopsInference::release_device(handler);
    }
    TopsInference::topsInference_finish();
  }

private:
  void* handler;
};

struct Buffer {
  Buffer(std::string name, tensorflow::DataType data_type)
      : name(name), data_type(data_type), byte_1batch_size(0), byte_size(0), buffer_on_device(nullptr) {
  }
  tensorflow::Status init(const adlik::serving::DimsList& dims, const adlik::serving::ModelConfigProto& config);
  Buffer(const Buffer& buffer) = delete;
  Buffer(Buffer&& buffer)
      : name(buffer.name),
        data_type(buffer.data_type),
        byte_1batch_size(buffer.byte_1batch_size),
        shape(buffer.shape),
        buffer_on_device(buffer.buffer_on_device) {
    buffer.buffer_on_device = nullptr;
  }
  Buffer& operator=(const Buffer&) = delete;
  Buffer& operator=(Buffer&& buffer) = delete;
  ~Buffer() {
    if (buffer_on_device != nullptr) {
      TopsInference::mem_free(buffer_on_device);
    }
  }

  std::string name;
  tensorflow::DataType data_type;
  size_t byte_1batch_size;
  size_t byte_size;
  std::vector<size_t> shape;
  void* buffer_on_device;
};

tensorflow::Status Buffer::init(const adlik::serving::DimsList& dims, const adlik::serving::ModelConfigProto& config) {
  shape.push_back(config.max_batch_size());
  for (auto dim : dims)
    shape.push_back(dim);
  size_t typesize = GetDataTypeSize(data_type);
  if (typesize <= 0)
    return tensorflow::errors::InvalidArgument("unknow data_type ", data_type);
  byte_1batch_size = std::accumulate(++shape.begin(), shape.end(), 1, std::multiplies<int>()) * typesize;
  byte_size = byte_1batch_size * config.max_batch_size();

  TopsInference::mem_alloc(&buffer_on_device, byte_size);

  return tensorflow::Status::OK();
}

struct Instance : BatchProcessor {
  Instance(const std::string& name, const adlik::serving::ModelConfigProto& config)
      : name(name), config(config), engine(nullptr), stream(nullptr) {
  }
  Instance(const Instance& instance) = delete;
  Instance(Instance&&) = delete;
  Instance& operator=(const Instance&) = delete;
  Instance& operator=(Instance&& instance) = delete;
  ~Instance() {
    if (stream != nullptr) {
      TopsInference::destroy_stream(stream);
    }
    if (engine != nullptr) {
      TopsInference::release_engine(engine);
    }
    INFO_LOG << "~Instance()";
  };
  tensorflow::Status init(const std::string modelFileName);
  static void setDevice() {
    static DeviceSetter setter;
    setter.func();
  }

private:
  const std::string name;
  const adlik::serving::ModelConfigProto config;
  TopsInference::IEngine* engine;
  TopsInference::topsInferStream_t stream;
  std::vector<Buffer> inputs;
  std::vector<Buffer> outputs;
  std::vector<double> times;

  tensorflow::Status processBatch(Batch<BatchingMessageTask>&) override;
  tensorflow::Status mergeInputs(Batch<BatchingMessageTask>&);
  tensorflow::Status splitOutputs(Batch<BatchingMessageTask>&);
};

tensorflow::Status Instance::init(std::string modelFileName) {
  TopsInference::topsInference_init();
  setDevice();
  TopsInference::create_stream(&stream);
  engine = TopsInference::create_engine();
  try {
    engine->loadExecutable(modelFileName.c_str());
  } catch (std::exception& e) {
    INFO_LOG << "Cannot load the model";
    return tensorflow::errors::Internal("Cannot load the model ", e.what());
  }

  // init inputs
  for (int i = 0; i < config.input_size(); ++i) {
    auto model_input = config.input(i);
    inputs.emplace_back(model_input.name(), model_input.data_type());
    MY_RETURN_IF_ERROR_TF(inputs.back().init(model_input.dims(), config));
  }

  // init outputs
  for (int i = 0; i < config.output_size(); ++i) {
    auto model_output = config.output(i);
    outputs.emplace_back(model_output.name(), model_output.data_type());
    MY_RETURN_IF_ERROR_TF(outputs.back().init(model_output.dims(), config));
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
  std::vector<void*> inputs_buffer_bound;
  for (auto& input : inputs) {
    inputs_buffer_bound.push_back(input.buffer_on_device);
  }
  std::vector<void*> outputs_buffer_bound;
  for (auto& output : outputs) {
    outputs_buffer_bound.push_back(output.buffer_on_device);
  }
  bool rtn = engine->run_with_batch(config.max_batch_size(),
                                    inputs_buffer_bound.data(),
                                    outputs_buffer_bound.data(),
                                    TopsInference::BufferType::TIF_ENGINE_RSC_IN_DEVICE_OUT_DEVICE,
                                    stream);
  if (!rtn) {
    return tensorflow::errors::Internal("Faild run in enflame reference.");
  }
  status = splitOutputs(payloads);
  TopsInference::synchronize_stream(stream);
  if (!status.ok()) {
    ERR_LOG << "After predict, error message: " << status.error_message();
  }
  return status;
}

tensorflow::Status Instance::mergeInputs(Batch<BatchingMessageTask>& batch) {
  size_t offsetSize = 0;
  tensorflow::Status status = tensorflow::Status::OK();
  for (int i = 0; i < batch.num_tasks(); ++i) {
    const BatchingMessageTask& task = batch.task((int)i);
    const PredictRequestProvider* requestProvider = task.request;
    const size_t batchSize = requestProvider->batchSize();
    auto func = [&](const std::string& name, const tensorflow::TensorProto& tensor) {
      const void* content = tensor.tensor_content().c_str();
      void* storage_data = const_cast<void*>(content);
      size_t size = tensor.tensor_content().size();
      for (auto& input : inputs) {
        if (name == input.name) {
          size_t actualByteSizePerSample = size / batchSize;
          if (actualByteSizePerSample != input.byte_1batch_size) {
            status = tensorflow::errors::InvalidArgument(
                "unexpected size of 1 batch ", actualByteSizePerSample, ", expecting ", input.byte_1batch_size);
            return false;
          }
          size_t offsetByteSize = actualByteSizePerSample * offsetSize;
          if (offsetByteSize + size > input.byte_size) {
            status = tensorflow::errors::InvalidArgument("unexpected size ",
                                                         offsetByteSize + size,
                                                         " biggger than input buffer space, expecting ",
                                                         input.byte_size);
            return false;
          }
          if (!TopsInference::mem_copy_async(storage_data,
                                             (char*)input.buffer_on_device + offsetByteSize,
                                             size,
                                             TopsInference::MemcpyKind::TIF_MEMCPY_HOST_TO_DEVICE,
                                             stream))
            status = tensorflow::errors::Internal("Fail copy from request to input buffer,");
          return true;
        }
      }
      return false;
    };
    requestProvider->visitInputs(func);
    offsetSize += batchSize;
  }
  return status;
}

tensorflow::Status Instance::splitOutputs(Batch<BatchingMessageTask>& batch) {
  for (auto& output : outputs) {
    size_t offsetByteSize = 0;

    for (int i = 0; i < batch.num_tasks(); ++i) {
      adlik::serving::DimsList dims;
      if (!ConvertDims(output.shape, dims)) {
        INFO_LOG << "Invalid buffer dims " << output.name << std::endl;
      }
      const BatchingMessageTask& task = batch.task(i);
      const PredictRequestProvider* requestProvider = task.request;
      PredictResponseProvider* responseProvider = task.response;
      const size_t out_size = requestProvider->batchSize() * output.byte_1batch_size;
      void* out = responseProvider->addOutput(output.name, output.data_type, dims, out_size);
      if (out == nullptr) {
        // maybe not need this output
      } else {
        if (offsetByteSize + out_size > output.byte_size) {
          return tensorflow::errors::InvalidArgument("unexpected size ",
                                                     offsetByteSize + out_size,
                                                     " for inference output '",
                                                     output.name,
                                                     "', expecting maximum",
                                                     output.byte_size);
        } else {
          if (!TopsInference::mem_copy_async((char*)output.buffer_on_device + offsetByteSize,
                                             out,
                                             out_size,
                                             TopsInference::MemcpyKind::TIF_MEMCPY_DEVICE_TO_HOST,
                                             stream))
            return tensorflow::errors::Internal("failed to copy output values from DTU for output ", output.name);
        }
      }
      offsetByteSize += out_size;
    }
  }
  return tensorflow::Status::OK();
}

tensorflow::Status createInstance(const std::string& instance_name,
                                  const ModelConfig& config,
                                  std::string modelFileName,
                                  std::unique_ptr<BatchProcessor>* model) {
  std::unique_ptr<Instance> instance = std::make_unique<Instance>(instance_name, config);
  TF_RETURN_IF_ERROR(instance->Instance::init(modelFileName));
  *model = std::move(instance);
  return tensorflow::Status::OK();
}

}  // namespace

EnflameModel::EnflameModel(const ModelConfig& config, const ModelId& model_id) : config(config), model_id(model_id) {
}

tensorflow::Status EnflameModel::init() {
  std::string filePath = config.getModelPath(model_id);
  // 1.read model file
  INFO_LOG << "Read the enflame model";
  std::string modelFileName = tensorflow::io::JoinPath(filePath, "model.exec");
  // 2 instance
  for (const auto& group : config.instance_group()) {
    for (int i = 0; i != group.count(); ++i) {
      std::unique_ptr<BatchProcessor> instance;
      std::string instance_name = group.name() + "_" + std::to_string(i);
      TF_RETURN_IF_ERROR(createInstance(instance_name, config, modelFileName, &instance));
      add(std::move(instance));
    }
  }

  return tensorflow::Status::OK();
}

cub::Status EnflameModel::create(const ModelConfig& model_config,
                                 const ModelId& model_id,
                                 std::unique_ptr<EnflameModel>* model_ptr) {
  ModelConfig copied_config(model_config);
  MY_RETURN_IF_ERROR(NormalizeModelConfig(copied_config));
  MY_RETURN_IF_ERROR(ValidateModelConfig(copied_config));

  model_ptr->reset(new EnflameModel(copied_config, model_id));
  auto status = (*model_ptr)->init();
  if (!status.ok()) {
    model_ptr->reset();
  }

  INFO_LOG << "After enflame inference, status: " << status.error_message();
  return cub::Status(status.code());
}

}  // namespace topsinference_runtime