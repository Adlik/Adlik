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
using tensorflow::Status;
using tensorflow::TensorProto;
using tensorflow::errors::Internal;
using tensorflow::errors::InvalidArgument;
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

  for (auto& entry : inputContextMap) {
    entry.second.commit(interpreter);
  }

  return Status::OK();
}

Status splitOutputs(const Interpreter& interpreter,
                    absl::Span<OutputContext>& outputContexts,
                    Batch<BatchingMessageTask>& batch) {
  const auto numTasks = batch.num_tasks();
  const auto totalSamples = batch.size();

  for (auto& context : outputContexts) {
    const auto& tensor = *interpreter.tensor(context.tensorIndex);
    const auto& dims = *tensor.dims;
    const auto& dimsList = context.calculateDimsList(dims);

    if (dims.size > 0) {
      if (batch.size() == static_cast<size_t>(dims.data[0])) {
        const auto sampleElements =
            static_cast<size_t>(std::accumulate(dims.data + 1, dims.data + dims.size, 1, std::multiplies<int>{}));

        auto firstElement = size_t{0};

        for (auto taskIndex = 0; taskIndex != numTasks; ++taskIndex) {
          const auto& task = batch.task(taskIndex);
          auto* const buffer = task.response->addOutput(context.getName(), context.dataType, dimsList);
          const auto batchElements = sampleElements * task.size();

          if (buffer) {
            context.readBatch(tensor, firstElement, batchElements, *buffer);
          }

          firstElement += batchElements;
        }
      } else {
        return InvalidArgument("Output batch size does not match requested batch size");
      }
    } else {
      for (auto taskIndex = 0; taskIndex != numTasks; ++taskIndex) {
        if (batch.task(taskIndex).response->addOutput(context.getName(), context.dataType, dimsList)) {
          return InvalidArgument("Scalar output is not supported");
        }
      }
    }
  }

  return Status::OK();
}
}  // namespace

Status processTensorFlowLiteTask(Interpreter& interpreter,
                                 unordered_map<string_view, InputContext, Hash<string_view>>& inputContextMap,
                                 absl::Span<OutputContext> outputContexts,
                                 Batch<BatchingMessageTask>& batch) {
  TF_RETURN_IF_ERROR(mergeInputs(interpreter, inputContextMap, batch));

  if (interpreter.Invoke() != TfLiteStatus::kTfLiteOk) {
    return Internal("Failed to invoke interpreter");
  }

  TF_RETURN_IF_ERROR(splitOutputs(interpreter, outputContexts, batch));

  return Status::OK();
}
}  // namespace serving
}  // namespace adlik
