// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_engine.h"

#include "adlik_serving/runtime/provider/predict_request_provider.h"
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
using tensorflow::gtl::MakeCleanup;
using tflite::Interpreter;

namespace adlik {
namespace serving {
namespace {
Status setInputs(Interpreter& interpreter,
                 const unordered_map<string_view, InputContext, Hash<string_view>>& inputContextMap,
                 Batch<BatchingMessageTask>& batch) {
  auto cleanUp = MakeCleanup([&] {
    for (const auto& entry : inputContextMap) {
      entry.second.bytesWrittenCache = 0;
    }
  });

  auto numTasks = batch.num_tasks();

  for (auto i = 0; i != numTasks; ++i) {
    const auto& task = batch.task(i);

    auto status = Status::OK();

    task.request->visitInputs([&](const string& name, const TensorProto& tensor) {
      const auto& context = inputContextMap.at(name);
      const auto& tensorContent = tensor.tensor_content();

      status = copyTensorProtoToTfLiteTensor(tensor, *interpreter.tensor(context.tensorIndex));

      auto isOk = status.ok();

      if (isOk) {
        context.bytesWrittenCache += tensorContent.length();
      }

      return isOk;
    });

    if (!status.ok()) {
      return status;
    }
  }
}
}  // namespace

Status processTensorFlowLiteTask(Interpreter& interpreter,
                                 const unordered_map<string_view, InputContext, Hash<string_view>>& inputContextMap,
                                 Batch<BatchingMessageTask>& batch) {
  TF_RETURN_IF_ERROR(setInputs(interpreter, inputContextMap, batch));

  throw std::logic_error("Not implemented");
}
}  // namespace serving
}  // namespace adlik
