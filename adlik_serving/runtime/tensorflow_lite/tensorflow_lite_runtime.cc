// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/batching/batching_model.h"
#include "adlik_serving/runtime/batching/composite_batch_processor.h"
#include "adlik_serving/runtime/util/unique_batcher_runtime_helper.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

namespace {

using adlik::serving::Batch;
using adlik::serving::BatchingMessageTask;
using adlik::serving::BatchingModel;
using adlik::serving::BatchProcessor;
using adlik::serving::CompositeBatchProcessor;
using adlik::serving::ModelConfig;
using adlik::serving::ModelId;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using tflite::FlatBufferModel;
using tflite::Interpreter;

class TensorFlowLiteBatchProcessor : public BatchProcessor {
  shared_ptr<FlatBufferModel> model;  // Make sure the model is alive when interpreter is alive.
  unique_ptr<Interpreter> interpreter;

  virtual tensorflow::Status processBatch(Batch<BatchingMessageTask>& batch) override {
    throw std::logic_error("TODO");
  }

public:
  TensorFlowLiteBatchProcessor(shared_ptr<FlatBufferModel> model, unique_ptr<Interpreter> interpreter)
      : model(std::move(model)), interpreter(std::move(interpreter)) {
  }
};

class TensorFlowLiteModel : public CompositeBatchProcessor, public BatchingModel {
public:
  IMPL_ROLE_NS(adlik::serving, BatchProcessor);

  static cub::Status create(const ModelConfig& modelConfig,
                            const ModelId& modelId,
                            unique_ptr<TensorFlowLiteModel>* model) {
    auto result = make_unique<TensorFlowLiteModel>();
    auto modelPath = modelConfig.getModelPath(modelId);
    shared_ptr<FlatBufferModel> flatBufferModel = FlatBufferModel::BuildFromFile(modelPath.c_str());
    tflite::ops::builtin::BuiltinOpResolver opResolver;
    tflite::InterpreterBuilder interpreterBuilder{*flatBufferModel, opResolver};

    for (const auto& instanceGroup : modelConfig.instance_group()) {
      for (int i = 0; i != instanceGroup.count(); ++i) {
        unique_ptr<Interpreter> interpreter;

        if (interpreterBuilder(&interpreter, 1) == TfLiteStatus::kTfLiteOk) {
          result->add(make_unique<TensorFlowLiteBatchProcessor>(flatBufferModel, std::move(interpreter)));
        } else {
          return cub::Failure;
        }
      }
    }

    *model = std::move(result);

    return cub::Success;
  }
};

DEFINE_UNIQUE_BATCHER_RUNTIME(TensorFlowLite, TensorFlowLiteModel);

}  // namespace
