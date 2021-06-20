// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tvm/tvm_model.h"

#include <dlpack/dlpack.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/registry.h>

#include "adlik_serving/framework/domain/model_config_helper.h"
#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "adlik_serving/runtime/provider/predict_response_provider.h"
#include "cub/env/fs/path.h"
#include "tensorflow/core/lib/core/errors.h"

namespace tvm_runtime {

using namespace adlik::serving;

namespace {

#define MY_EXPECT_SUCC_CALL(call) \
  do {                            \
    auto __status = call;         \
    if (__CUB_FAILED(__status)) { \
      return __status;            \
    }                             \
  } while (0)

struct TvmInstance : BatchProcessor {
  TvmInstance(const ModelConfigProto& config) : config(config) {
  }

  cub::Status load(const std::string& modelPath);

  using MyBatch = Batch<BatchingMessageTask>;
  OVERRIDE(tensorflow::Status processBatch(MyBatch&));

private:
  struct MyBlob {
    std::string name;
    std::vector<char> buffer;
    size_t size;
    size_t offset;

    tvm::runtime::NDArray array;
  };

  std::vector<int64_t> getShape(const size_t batchSize, const DimsList& dims) {
    std::vector<int64_t> shape;
    shape.push_back(batchSize);
    for (auto dim : dims) {
      shape.push_back(dim);
    }
    return std::move(shape);
  }

  void initInput() {
    DLContext dev{kDLCPU, 0};
    for (const auto& input : config.input()) {
      MyBlob blob;

      blob.array = tvm::runtime::NDArray::Empty(
          getShape(config.max_batch_size(), input.dims()), DLDataType{kDLFloat, 32, 1}, dev);
      blob.name = input.name();
      blob.offset = 0;
      blob.size = GetSize(config.max_batch_size(), input.data_type(), input.dims());
      blob.buffer.resize(blob.size);
      inputs.push_back(std::move(blob));
    }
  }

  void initOutput() {
    DLContext dev{kDLCPU, 0};
    for (const auto& output : config.output()) {
      MyBlob blob;
      blob.array = tvm::runtime::NDArray::Empty(
          getShape(config.max_batch_size(), output.dims()), DLDataType{kDLFloat, 32, 1}, dev);
      blob.name = output.name();
      blob.offset = 0;
      blob.size = GetSize(config.max_batch_size(), output.data_type(), output.dims());
      blob.buffer.resize(blob.size);
      outputs.push_back(std::move(blob));
    }
  }

  tensorflow::Status mergeInputs(MyBatch& batch);
  tensorflow::Status splitOutputs(MyBatch& batch);

  const ModelConfigProto& config;

  tvm::runtime::Module mod_factory;
  tvm::runtime::Module gmod;
  tvm::runtime::PackedFunc set_input;
  tvm::runtime::PackedFunc get_output;
  tvm::runtime::PackedFunc run;

  std::vector<MyBlob> inputs;
  std::vector<MyBlob> outputs;
};

cub::Status TvmInstance::load(const std::string& modelPath) {
  DLContext ctx{kDLCPU, 0};
  mod_factory = tvm::runtime::Module::LoadFromFile(modelPath);
  gmod = mod_factory.GetFunction("default")(ctx);
  set_input = gmod.GetFunction("set_input");
  get_output = gmod.GetFunction("get_output");
  run = gmod.GetFunction("run");

  initInput();
  initOutput();

  return cub::Success;
}

tensorflow::Status TvmInstance::processBatch(MyBatch& batch) {
  auto status = mergeInputs(batch);
  if (!status.ok()) {
    // ERR_LOG << "Merge Inputs error: " << status.error_message();
    return status;
  }
  run();
  status = splitOutputs(batch);
  return status;
}

tensorflow::Status TvmInstance::mergeInputs(MyBatch& batch) {
  for (auto& i : inputs) {
    i.offset = 0;
  }
  tensorflow::Status status = tensorflow::Status::OK();

  for (int i = 0; i < batch.num_tasks(); ++i) {
    const auto& task = batch.task(i);
    auto func = [&](const std::string& name, const tensorflow::TensorProto& tensor) {
      const void* content = tensor.tensor_content().c_str();
      size_t totalByteSize = tensor.tensor_content().size();

      for (auto& item : inputs) {
        if (item.name == name) {
          if (item.offset + totalByteSize > item.size) {
            status = tensorflow::errors::InvalidArgument("unexpected size ",
                                                         item.offset + totalByteSize,
                                                         " biggger than input blob space, expecting ",
                                                         item.size);
            return false;
          }
          std::memcpy(item.buffer.data() + item.offset, content, totalByteSize);
          item.offset += totalByteSize;
          return true;
        }
      }
      return false;
    };
    task.request->visitInputs(func);
  }
  if (!status.ok()) {
    return status;
  }

  for (auto& input : inputs) {
    input.array.CopyFromBytes(input.buffer.data(), input.size);
    set_input(input.name, input.array);
  }
  return status;
}

tensorflow::Status TvmInstance::splitOutputs(MyBatch& batch) {
  for (size_t i = 0; i < outputs.size(); ++i) {
    auto& item = outputs[i];
    item.offset = 0;
    get_output(i, item.array);
    item.array.CopyToBytes(item.buffer.data(), item.size);

    size_t byteSizePerSample = item.size / config.max_batch_size();
    tensorflow::DataType dtype = config.output((int)i).data_type();
    adlik::serving::DimsList dims = config.output((int)i).dims();

    for (int i = 0; i < batch.num_tasks(); ++i) {
      const auto* requestProvider = batch.task(i).request;
      auto* responseProvider = batch.task(i).response;
      const size_t expectedByteSize = requestProvider->batchSize() * byteSizePerSample;
      void* content = responseProvider->addOutput(item.name, dtype, dims, expectedByteSize);
      if (content == nullptr) {
        // maybe not need this output
      } else {
        std::memcpy(content, item.buffer.data() + item.offset, expectedByteSize);
      }
      item.offset += expectedByteSize;
    }
  }

  return tensorflow::Status::OK();
}

cub::Status createInstance(const std::string& modelPath,
                           const ModelConfig& config,
                           std::unique_ptr<BatchProcessor>* model) {
  std::unique_ptr<TvmInstance> raw = std::make_unique<TvmInstance>(config);
  MY_EXPECT_SUCC_CALL(raw->load(modelPath));
  *model = std::move(raw);
  return cub::Success;
}

}  // namespace

TvmModel::TvmModel(const adlik::serving::ModelConfig& config, const adlik::serving::ModelId& model_id)
    : config(config), model_id(model_id) {
}

cub::Status TvmModel::init() {
  std::string modelPath = cub::paths(config.getModelPath(model_id), "model.so");

  for (const auto& group : config.instance_group()) {
    for (int i = 0; i != group.count(); ++i) {
      std::unique_ptr<BatchProcessor> instance;
      MY_EXPECT_SUCC_CALL(createInstance(modelPath, config, &instance));
      add(std::move(instance));
    }
  }
  return cub::Success;
}

cub::Status TvmModel::create(const ModelConfig& model_config,
                             const ModelId& model_id,
                             std::unique_ptr<TvmModel>* model_ptr) {
  ModelConfig copied_config(model_config);
  NormalizeModelConfig(copied_config);

  model_ptr->reset(new TvmModel(copied_config, model_id));
  auto status = (*model_ptr)->init();
  if (!CUB_IS_SUCC_STATUS(status)) {
    model_ptr->reset();
  }
  return status;
}

}  // namespace tvm_runtime
