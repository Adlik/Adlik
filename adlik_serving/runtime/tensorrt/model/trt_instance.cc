// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorrt/model/trt_instance.h"

#include <NvInfer.h>
#include <cuda_runtime_api.h>

#include <algorithm>

#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/framework/manager/time_stats.h"
#include "adlik_serving/runtime/batching/batch_processor.h"
#include "adlik_serving/runtime/batching/batching_message_task.h"
#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "adlik_serving/runtime/provider/predict_response_provider.h"
#include "adlik_serving/runtime/tensorrt/model/trt_util.h"
#include "cub/log/log.h"
#include "tensorflow/core/lib/core/errors.h"

namespace tensorrt {

using namespace adlik::serving;

namespace {

struct Logger : public nvinfer1::ILogger {
  void log(Severity severity, const char* msg) override {
    switch (severity) {
      case Severity::kINTERNAL_ERROR:
        ERR_LOG << msg;
        break;
      case Severity::kERROR:
        ERR_LOG << msg;
        break;
      case Severity::kWARNING:
        WARN_LOG << msg;
        break;
      case Severity::kINFO:
        INFO_LOG << msg;
        break;
    }
  }
};

Logger tensorrt_logger;

struct DestroyDeleter {
  template <typename T>
  void operator()(T* p) const {
    p->destroy();
  }
};

struct StreamDeleter {
  void operator()(CUstream_st* p) const {
    cudaError_t err = cudaStreamDestroy(p);
    if (err != cudaSuccess) {
      ERR_LOG << "Failed to destroy cuda stream: " << cudaGetErrorString(err);
    }
  }
};

template <typename T>
using TrtPtr = std::unique_ptr<T, DestroyDeleter>;

struct Instance : BatchProcessor {
  Instance(const ModelConfigProto&, const std::string& name, const int gpu_device);
  ~Instance();

  tensorflow::Status init(const std::vector<char>& model_data);

private:
  TF_DISALLOW_COPY_AND_ASSIGN(Instance);
  tensorflow::Status loadPlan(const std::vector<char>& model_data);
  tensorflow::Status allocBuffer();
  tensorflow::Status createExecuteContext();

  tensorflow::Status initializeInputBindings(const ::google::protobuf::RepeatedPtrField<ModelInput>& ios);
  tensorflow::Status initializeOutputBindings(const ::google::protobuf::RepeatedPtrField<ModelOutput>& ios);

  using MyBatch = Batch<BatchingMessageTask>;
  OVERRIDE(tensorflow::Status processBatch(MyBatch&));
  tensorflow::Status mergeInputs(MyBatch&);
  tensorflow::Status splitOutputs(MyBatch&);

  const ModelConfigProto& config;

  const std::string name;

  // The GPU index active when this context was created.
  const int gpu_device;

  const int max_batch_size;

  TrtPtr<nvinfer1::IRuntime> runtime;
  TrtPtr<nvinfer1::ICudaEngine> engine;
  TrtPtr<nvinfer1::IExecutionContext> context;

  size_t num_inputs;

  // For each binding index of the TensorRT engine, the size of the
  // corresponding tensor and pointer to the CUDA buffer for the
  // tensor. These are arrays with size equal to number of bindings.
  std::unique_ptr<uint64_t[]> byte_sizes;
  std::unique_ptr<void*[]> buffers;

  // The stream where operations are executed.
  std::unique_ptr<CUstream_st, StreamDeleter> stream;
};
////////////////////////////////////////////////////////////////////////////////////

Instance::Instance(const ModelConfigProto& config, const std::string& name, const int gpu_device)
    : config(config), name(name), gpu_device(gpu_device), max_batch_size(config.max_batch_size()), num_inputs(0) {
}

Instance::~Instance() {
  INFO_LOG << "~Instance::Instance ";

  if (buffers) {
    for (int i = 0; i < engine->getNbBindings(); ++i) {
      if (buffers[i] != nullptr) {
        cudaError_t err = cudaFree(buffers[i]);
        if (err != cudaSuccess) {
          ERR_LOG << "Failed to free cuda memory for '" << name << "': " << cudaGetErrorString(err);
        }
      }
    }
  }
}

tensorflow::Status Instance::init(const std::vector<char>& model_data) {
  TF_RETURN_IF_ERROR(loadPlan(model_data));
  TF_RETURN_IF_ERROR(allocBuffer());
  TF_RETURN_IF_ERROR(createExecuteContext());
  return tensorflow::Status::OK();
}

tensorflow::Status Instance::loadPlan(const std::vector<char>& model_data) {
  if (runtime || engine) {
    return tensorflow::errors::Internal(
        "TensorRt runtime or engine is not null, maybe should destroy before load "
        "new model.");
  }
  auto cuerr = cudaSetDevice(gpu_device);
  if (cuerr != cudaSuccess) {
    return tensorflow::errors::Internal("unable to set device for ", config.name(), ": ", cudaGetErrorString(cuerr));
  }

  runtime.reset(nvinfer1::createInferRuntime(tensorrt_logger));
  if (!runtime) {
    return tensorflow::errors::Internal("Unable to create TensorRT runtime");
  }

  engine.reset(runtime->deserializeCudaEngine(&model_data[0], model_data.size(), nullptr));
  if (!engine) {
    return tensorflow::errors::Internal("Unable to create TensorRT engine");
  }

  if (max_batch_size > engine->getMaxBatchSize()) {
    return tensorflow::errors::InvalidArgument("unexpected configuration maximum batch size ",
                                               max_batch_size,
                                               " for '",
                                               name,
                                               "', model maximum is ",
                                               engine->getMaxBatchSize());
  }
  return tensorflow::Status::OK();
}

tensorflow::Status Instance::allocBuffer() {
  const int num_expected_bindings = engine->getNbBindings();
  byte_sizes = std::make_unique<uint64_t[]>(num_expected_bindings);
  buffers = std::make_unique<void*[]>(num_expected_bindings);

  TF_RETURN_IF_ERROR(initializeInputBindings(config.input()));
  TF_RETURN_IF_ERROR(initializeOutputBindings(config.output()));
  // Make sure every index is initialized.
  for (int i = 0; i < num_expected_bindings; ++i) {
    if (buffers[i] == nullptr) {
      return tensorflow::errors::InvalidArgument("expected configuration for ",
                                                 (engine->bindingIsInput(i) ? "input" : "output"),
                                                 " '",
                                                 engine->getBindingName(i),
                                                 "' for ",
                                                 config.name());
    }
  }
  return tensorflow::Status::OK();
}

tensorflow::Status Instance::createExecuteContext() {
  context.reset(engine->createExecutionContext());
  if (!context) {
    return tensorflow::errors::Internal("unable to create TensorRT context");
  }

  // Create CUDA stream associated with the execution context, should call "cudaDeviceGetStreamPriorityRange" and
  // get numerical values that correspond to the least and greatest stream priorities. But now we use default "0"
  int cuda_stream_priority = 0;
  CUstream_st* temp_stream;
  auto cuerr = cudaStreamCreateWithPriority(&temp_stream, cudaStreamDefault, cuda_stream_priority);
  if (cuerr != cudaSuccess) {
    return tensorflow::errors::Internal("unable to create stream for ", config.name(), ": ", cudaGetErrorString(cuerr));
  }
  stream.reset(temp_stream);
  return tensorflow::Status::OK();
}

tensorflow::Status Instance::initializeInputBindings(const ::google::protobuf::RepeatedPtrField<ModelInput>& ios) {
  for (const auto& io : ios) {
    TF_RETURN_IF_ERROR(ValidateModelInput(io));

    int index = engine->getBindingIndex(io.name().c_str());
    if (index < 0) {
      return tensorflow::errors::NotFound("input '", io.name(), "' not found for ", name);
    }

    if (buffers[index] != nullptr) {
      return tensorflow::errors::InvalidArgument(
          "input '", io.name(), "' has already appeared as an ", "input or output for ", name);
    }

    if (!engine->bindingIsInput(index)) {
      return tensorflow::errors::InvalidArgument(
          "input '", io.name(), "' is expected to be an output in model for ", name);
    }

    tensorflow::DataType dt = ConvertDatatype(engine->getBindingDataType(index));
    if (dt != io.data_type()) {
      return tensorflow::errors::InvalidArgument("input '",
                                                 io.name(),
                                                 "' datatype is ",
                                                 DataType_Name(io.data_type()),
                                                 ", model specifies ",
                                                 DataType_Name(dt),
                                                 " for ",
                                                 name);
    }

    nvinfer1::Dims dims = engine->getBindingDimensions(index);
    if (!CompareDims(dims, io.dims())) {
      return tensorflow::errors::InvalidArgument("input '",
                                                 io.name(),
                                                 "' dims ",
                                                 DimsDebugString(dims.d),
                                                 " don't match configuration dims ",
                                                 DimsDebugString(io.dims()),
                                                 " for ",
                                                 name);
    }

    const uint64_t byte_size = ::adlik::serving::GetSize(max_batch_size, dt, io.dims());
    if (byte_size == 0) {
      return tensorflow::errors::Internal("unable to calculate size for input '", io.name(), " for ", name);
    }

    // Allocate CUDA memory
    void* buffer;
    cudaError_t err = cudaMalloc(&buffer, byte_size);
    if (err != cudaSuccess) {
      return tensorflow::errors::Internal(
          "unable to allocate memory for input '", io.name(), " for ", name, ": ", cudaGetErrorString(err));
    }

    byte_sizes[index] = byte_size;
    buffers[index] = buffer;
    num_inputs++;
  }

  return tensorflow::Status::OK();
}

tensorflow::Status Instance::initializeOutputBindings(const ::google::protobuf::RepeatedPtrField<ModelOutput>& ios) {
  for (const auto& io : ios) {
    TF_RETURN_IF_ERROR(ValidateModelOutput(io));

    int index = engine->getBindingIndex(io.name().c_str());
    if (index < 0) {
      return tensorflow::errors::NotFound("output '", io.name(), "' not found for ", name);
    }

    if (buffers[index] != nullptr) {
      return tensorflow::errors::InvalidArgument(
          "output '", io.name(), "' has already appeared as an ", "input or output for ", name);
    }

    if (engine->bindingIsInput(index)) {
      return tensorflow::errors::InvalidArgument(
          "output '", io.name(), "' is expected to be an input in model for ", name);
    }

    tensorflow::DataType dt = ConvertDatatype(engine->getBindingDataType(index));
    if (dt != io.data_type()) {
      return tensorflow::errors::InvalidArgument("output '",
                                                 io.name(),
                                                 "' datatype is ",
                                                 DataType_Name(io.data_type()),
                                                 ", model specifies ",
                                                 DataType_Name(dt),
                                                 " for ",
                                                 name);
    }

    nvinfer1::Dims dims = engine->getBindingDimensions(index);
    if (!CompareDims(dims, io.dims())) {
      return tensorflow::errors::InvalidArgument("output '",
                                                 io.name(),
                                                 "' dims ",
                                                 DimsDebugString(dims.d),
                                                 " don't match configuration dims ",
                                                 DimsDebugString(io.dims()),
                                                 " for ",
                                                 name);
    }

    const uint64_t byte_size = adlik::serving::GetSize(max_batch_size, dt, io.dims());
    if (byte_size == 0) {
      return tensorflow::errors::Internal("unable to calculate size for output '", io.name(), " for ", name);
    }

    // Allocate CUDA memory
    void* buffer;
    cudaError_t err = cudaMalloc(&buffer, byte_size);
    if (err != cudaSuccess) {
      return tensorflow::errors::Internal(
          "unable to allocate memory for input '", io.name(), " for ", name, ": ", cudaGetErrorString(err));
    }

    byte_sizes[index] = byte_size;
    buffers[index] = buffer;
  }

  return tensorflow::Status::OK();
}

tensorflow::Status Instance::processBatch(MyBatch& payloads) {
  INFO_LOG << "Running " << name << " with " << payloads.size() << " request payloads";
  cudaSetDevice(gpu_device);
  if (payloads.size() == 0) {
    return tensorflow::Status::OK();
  }
  // total_batch_size can be 1 for models that don't support batching
  // (i.e. max_batch_size == 0).
  if ((payloads.size() != 1) && (payloads.size() > (size_t)max_batch_size)) {
    return tensorflow::errors::Internal(
        "dynamic batch size ", payloads.size(), " for '", name, "', max allowed is ", max_batch_size);
  }

  auto status = mergeInputs(payloads);
  if (!status.ok()) {
    INFO_LOG << status.error_message() << std::endl;
    return status;
  }

  adlik::serving::TimeStats stats("TrtModel::Instance::Run::TensorRT model " + name +
                                  ", run prediction on GPU batch=" + std::to_string(payloads.size()));

  if (!context->enqueue(payloads.size(), buffers.get(), stream.get(), nullptr)) {
    cudaStreamSynchronize(stream.get());
    return tensorflow::errors::Internal("unable to enqueue for inference ", name);
  }

  status = splitOutputs(payloads);
  cudaStreamSynchronize(stream.get());
  return status;
}

tensorflow::Status Instance::mergeInputs(MyBatch& batch) {
  tensorflow::Status status = tensorflow::Status::OK();

  for (int bindex = 0; bindex < engine->getNbBindings(); ++bindex) {
    if (!engine->bindingIsInput(bindex)) {
      continue;
    }
    const std::string& name = engine->getBindingName(bindex);
    const size_t batch1_byte_size = byte_sizes[bindex] / std::max(1, max_batch_size);
    size_t binding_copy_offset = 0;

    for (int i = 0; i < batch.num_tasks(); ++i) {
      auto* request = batch.task(i).request;
      const size_t expected_byte_size = request->batchSize() * batch1_byte_size;

      auto func = [&](const std::string& cur_name, const tensorflow::TensorProto& tensor) {
        const void* content = tensor.tensor_content().c_str();
        size_t content_byte_size = tensor.tensor_content().size();

        if (cur_name == name) {
          size_t copied_byte_size = 0;
          if (content == nullptr) {
            return false;
          }
          if ((binding_copy_offset + copied_byte_size + content_byte_size) > byte_sizes[bindex]) {
            status = tensorflow::errors::InvalidArgument("unexpected size ",
                                                         binding_copy_offset + copied_byte_size + content_byte_size,
                                                         " for inference input '",
                                                         name,
                                                         "', expecting ",
                                                         byte_sizes[bindex]);
            return false;
          }
          cudaError_t err =
              cudaMemcpyAsync(static_cast<char*>(buffers[bindex]) + binding_copy_offset + copied_byte_size,
                              content,
                              content_byte_size,
                              cudaMemcpyHostToDevice,
                              stream.get());
          if (err != cudaSuccess) {
            status = tensorflow::errors::Internal(
                "failed to copy input values to GPU for input '", name, "': ", cudaGetErrorString(err));
            return false;
          }

          copied_byte_size += content_byte_size;
          if (copied_byte_size != expected_byte_size) {
            status = tensorflow::errors::Internal(
                "expected ", expected_byte_size, " of data for inference input '", name, "', got ", copied_byte_size);
          }

          return false;  // stop visit
        } else {
          return true;  // not this input, continue visit
        }
      };

      request->visitInputs(func);
      binding_copy_offset += expected_byte_size;
    }
  }
  return status;
}

tensorflow::Status Instance::splitOutputs(MyBatch& batch) {
  for (int bindex = 0; bindex < engine->getNbBindings(); ++bindex) {
    if (engine->bindingIsInput(bindex)) {
      continue;
    }

    const std::string& name = engine->getBindingName(bindex);
    tensorflow::DataType dtype = ConvertDatatype(engine->getBindingDataType(bindex));
    nvinfer1::Dims mb_dims = engine->getBindingDimensions(bindex);
    adlik::serving::DimsList dims;
    ConvertDims(mb_dims, dims);

    const size_t batch1_byte_size = (byte_sizes[bindex] / std::max(1, max_batch_size));
    size_t binding_copy_offset = 0;

    for (int i = 0; i < batch.num_tasks(); ++i) {
      auto req = batch.task(i).request;
      const size_t expected_byte_size = req->batchSize() * batch1_byte_size;

      void* content = batch.task(i).response->addOutput(name, dtype, dims, expected_byte_size);
      if (content == nullptr) {
        // maybe not needed this output
        // payload.compute_status_ = tensorflow::errors::Internal(
        //     "no buffer to accept output values for output '", name, "'");
      } else {
        if ((binding_copy_offset + expected_byte_size) > byte_sizes[bindex]) {
          return tensorflow::errors::InvalidArgument("unexpected size ",
                                                     binding_copy_offset + expected_byte_size,
                                                     " for inference output '",
                                                     name,
                                                     "', expecting maximum",
                                                     byte_sizes[bindex]);
        } else {
          cudaError_t err = cudaMemcpyAsync(content,
                                            static_cast<char*>(buffers[bindex]) + binding_copy_offset,
                                            expected_byte_size,
                                            cudaMemcpyDeviceToHost,
                                            stream.get());
          if (err != cudaSuccess) {
            return tensorflow::errors::Internal(
                "failed to copy output values from GPU for output '", name, "': ", cudaGetErrorString(err));
          }
        }
      }
      binding_copy_offset += expected_byte_size;
    }
  }
  return tensorflow::Status::OK();
}

}  // namespace

tensorflow::Status CreateTrtInstance(const ModelConfigProto& config,
                                     const std::string& name,
                                     const int gpu_device,
                                     const std::vector<char>& model_data,
                                     std::unique_ptr<BatchProcessor>* model) {
  std::unique_ptr<Instance> raw = std::make_unique<Instance>(config, name, gpu_device);
  TF_RETURN_IF_ERROR(raw->init(model_data));
  *model = std::move(raw);
  return tensorflow::Status::OK();
}

}  // namespace tensorrt
