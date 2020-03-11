// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_engine.h"

#include <functional>
#include <numeric>

#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "adlik_serving/runtime/provider/predict_response_provider.h"
#include "adlik_serving/runtime/tensorflow_lite/input_context.h"
#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/gtl/cleanup.h"

using absl::Hash;
using absl::string_view;
using std::string;
using std::unordered_map;
using std::vector;
using tensorflow::Status;
using tensorflow::TensorProto;
using tensorflow::errors::Code;
using tensorflow::gtl::MakeCleanup;
using tflite::Interpreter;

namespace adlik {
namespace serving {
namespace {
Status mergeInputs(Interpreter& interpreter,
                   unordered_map<string_view, InputContext, Hash<string_view>>& inputContextMap,
                   const Batch<BatchingMessageTask>& batch) {
  const auto cleanUp = MakeCleanup([&] {
    for (auto& entry : inputContextMap) {
      entry.second.reset();
    }
  });

  const auto numTasks = batch.num_tasks();

  for (auto i = 0; i != numTasks; ++i) {
    const auto& task = batch.task(i);

    auto status = Status::OK();

    task.request->visitInputs([&](const string& name, const TensorProto& tensorProto) {
      auto& context = inputContextMap.at(name);

      context.addInputTensor(interpreter, tensorProto);

      status = context.addInputTensor(interpreter, tensorProto);

      return status.ok();
    });

    TF_RETURN_IF_ERROR(status);
  }
}

Status splitOutputs(Interpreter& interpreter,
                    absl::Span<OutputContext>& outputContexts,
                    Batch<BatchingMessageTask>& batch) {
  const auto numTasks = batch.num_tasks();

  for (auto i = 0; i != numTasks; ++i) {
    const auto& task = batch.task(i);
    const auto batchSize = task.request->batchSize();
    auto& response = *task.response;

    for (auto& context : outputContexts) {
      const auto& tensor = *interpreter.tensor(context.tensorIndex);
      const auto dimsList = context.getDimsList(batchSize, *tensor.dims);

      throw std::logic_error("Not implemented");
    }
  }
}
}  // namespace

Status processTensorFlowLiteTask(Interpreter& interpreter,
                                 unordered_map<string_view, InputContext, Hash<string_view>>& inputContextMap,
                                 absl::Span<OutputContext> outputContexts,
                                 Batch<BatchingMessageTask>& batch) {
  TF_RETURN_IF_ERROR(mergeInputs(interpreter, inputContextMap, batch));

  if (interpreter.Invoke() != TfLiteStatus::kTfLiteOk) {
    return Status{Code::INTERNAL, "Failed to invoke interpreter"};
  }

  TF_RETURN_IF_ERROR(splitOutputs(interpreter, outputContexts, batch));

  return Status::OK();
}
}  // namespace serving
}  // namespace adlik
