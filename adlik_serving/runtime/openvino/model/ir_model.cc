// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/openvino/model/ir_model.h"

#include <stdlib.h>

#include <inference_engine.hpp>

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

using namespace InferenceEngine;
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

  tensorflow::Status load(const CNNNetwork& network, ExecutableNetwork& executableNetwork);

  using MyBatch = Batch<BatchingMessageTask>;
  OVERRIDE(tensorflow::Status processBatch(MyBatch&));

private:
  std::string name;
  adlik::serving::ModelConfigProto config;
  mutable InferRequest infer_request;
  BlobMap outputs;
  BlobMap inputs;

  tensorflow::Status setInputBlob(InputsDataMap inputsInfo);
  tensorflow::Status setOutputBlob(OutputsDataMap outputsInfo);

  tensorflow::Status mergeInputs(MyBatch&);
  tensorflow::Status splitOutputs(MyBatch&);
  Precision getInputDataType(const std::string& inputName);
  Precision getOutputDataType(const std::string& outputName);
  Layout getInputLayout(const std::string& inputName);
};

tensorflow::Status PluginLoader::load(const CNNNetwork& network, ExecutableNetwork& executableNetwork) {
  // 1.Prepare inputs and outputs format
  TF_RETURN_IF_ERROR(setInputBlob(network.getInputsInfo()));
  TF_RETURN_IF_ERROR(setOutputBlob(network.getOutputsInfo()));

  // 2. set inferrequest
  infer_request = executableNetwork.CreateInferRequest();
  infer_request.SetInput(inputs);
  infer_request.SetOutput(outputs);

  return tensorflow::Status::OK();
}

tensorflow::Status PluginLoader::setInputBlob(InputsDataMap inputsInfo) {
  for (auto& item : inputsInfo) {
    Precision itemPrecision = getInputDataType(item.first);
    Layout itemLayout = getInputLayout(item.first);
    INFO_LOG << "input layout:" << item.first << " " << itemLayout;
    SizeVector inputDims = item.second->getTensorDesc().getDims();
    INFO_LOG << "input dims:" << item.first << " " << inputDims.size();
    Blob::Ptr input = getBlob(itemPrecision, inputDims, itemLayout);
    if (input == nullptr) {
      return tensorflow::errors::Internal("input blob allocate failure in the model ", config.name());
    }
    inputs[item.first] = input;
  }
  return tensorflow::Status::OK();
}

tensorflow::Status PluginLoader::setOutputBlob(OutputsDataMap outputsInfo) {
  for (auto& item : outputsInfo) {
    Precision itemPrecision = getOutputDataType(item.first);
    Layout layout = item.second->getLayout();
    INFO_LOG << "output layout:" << item.first << " " << layout;
    SizeVector outputDims = item.second->getTensorDesc().getDims();
    INFO_LOG << "output dims:" << item.first << " " << outputDims.size();
    item.second->setPrecision(itemPrecision);
    Blob::Ptr output = getBlob(itemPrecision, outputDims, layout);
    if (output == nullptr) {
      return tensorflow::errors::Internal("output blob allocate failure in the model ", config.name());
    }
    outputs[item.first] = output;
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
  infer_request.StartAsync();
  infer_request.Wait(InferenceEngine::IInferRequest::RESULT_READY);
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
          Blob::Ptr inputPtr = item.second;
          size_t actualByteSizePerSample = totalByteSize / requestProvider->batchSize();
          size_t offsetByteSize = actualByteSizePerSample * offsetSize;
          size_t copyByteSize = actualByteSizePerSample * batchSize;
          if (offsetByteSize + totalByteSize > inputPtr->byteSize()) {
            status = tensorflow::errors::InvalidArgument("unexpected size ",
                                                         offsetByteSize + totalByteSize,
                                                         " biggger than input blob space, expecting ",
                                                         inputPtr->byteSize());
            return false;
          }
          return copyBuffer2Blob(content, inputPtr, copyByteSize, offsetByteSize);
        }
        return false;
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
    Blob::Ptr outputPtr = item.second;
    adlik::serving::DimsList dims;
    if (!ConvertDims(outputPtr->getTensorDesc().getDims(), dims)) {
      INFO_LOG << "Invalid blob dims " << item.first << std::endl;
    }
    tensorflow::DataType dtype = ConvertToTensorflowDatatype(outputPtr->getTensorDesc().getPrecision());
    size_t offsetByteSize = 0;
    size_t byteSizePerSample = outputPtr->byteSize() / outputPtr->getTensorDesc().getDims()[0];

    for (int i = 0; i < batch.num_tasks(); ++i) {
      const BatchingMessageTask& task = batch.task(i);
      const PredictRequestProvider* requestProvider = task.request;
      PredictResponseProvider* responseProvider = task.response;
      const size_t expectedByteSize = requestProvider->batchSize() * byteSizePerSample;
      void* content = responseProvider->addOutput(item.first, dtype, dims, expectedByteSize);

      if (content == nullptr) {
        // maybe not need this output
      } else {
        if (offsetByteSize + expectedByteSize > outputPtr->byteSize()) {
          return tensorflow::errors::InvalidArgument("unexpected size ",
                                                     offsetByteSize + expectedByteSize,
                                                     " for inference output '",
                                                     item.first,
                                                     "', expecting maximum",
                                                     outputPtr->byteSize());
        } else {
          if (!copyBlob2Buffer(content, outputPtr, expectedByteSize, offsetByteSize)) {
            return tensorflow::errors::Internal("failed to copy output values from CPU for output '", item.first);
          }
        }
      }
      offsetByteSize += expectedByteSize;
    }
  }
  return tensorflow::Status::OK();
}

Precision PluginLoader::getInputDataType(const std::string& inputName) {
  for (int i = 0; i < config.input_size(); i++) {
    if (inputName == config.input(i).name()) {
      return ConvertToOpenVinoDataType(config.input(i).data_type());
    }
  }
  return Precision::UNSPECIFIED;
}

Layout PluginLoader::getInputLayout(const std::string& inputName) {
  for (int i = 0; i < config.input_size(); i++) {
    if (inputName == config.input(i).name()) {
      return ConvertToOpenVinoLayout(config.input(i).format());
    }
  }
  return Layout::ANY;
}

Precision PluginLoader::getOutputDataType(const std::string& outputName) {
  for (int i = 0; i < config.output_size(); i++) {
    if (outputName == config.output(i).name()) {
      return ConvertToOpenVinoDataType(config.output(i).data_type());
    }
  }
  return Precision::UNSPECIFIED;
}

tensorflow::Status createInstance(const std::string& instance_name,
                                  const ModelConfig& config,
                                  const CNNNetwork& network,
                                  ExecutableNetwork& executableNetwork,
                                  std::unique_ptr<BatchProcessor>* model) {
  std::unique_ptr<PluginLoader> raw = std::make_unique<PluginLoader>(instance_name, config);
  TF_RETURN_IF_ERROR(raw->load(network, executableNetwork));
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
  std::string binFileName = tensorflow::io::JoinPath(filePath, "model.bin");
  std::string xmlFileName = tensorflow::io::JoinPath(filePath, "model.xml");
  // 2.load model
  CNNNetwork network;
  InferenceEngine::Core core;
  try {
    network = core.ReadNetwork(xmlFileName, binFileName);
  } catch (const details::InferenceEngineException& e) {
    INFO_LOG << "Cannot load the model";
    return tensorflow::errors::Internal("Cannot load the model ", e.what());
  }
  // 3.set batch size
  auto currentBatchSize = network.getBatchSize();
  if (currentBatchSize != config.max_batch_size()) {
    network.setBatchSize(config.max_batch_size());
  }
  // 4. set instance
  ExecutableNetwork executableNetwork = core.LoadNetwork(network, "CPU");
  for (const auto& group : config.instance_group()) {
    for (int i = 0; i != group.count(); ++i) {
      std::unique_ptr<BatchProcessor> instance;
      std::string instance_name = group.name() + "_" + std::to_string(i);
      TF_RETURN_IF_ERROR(createInstance(instance_name, config, network, executableNetwork, &instance));
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
