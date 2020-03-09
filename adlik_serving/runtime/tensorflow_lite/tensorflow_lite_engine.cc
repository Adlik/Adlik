// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_engine.h"

#include "adlik_serving/runtime/provider/predict_request_provider.h"

using absl::Hash;
using absl::string_view;
using std::unordered_map;
using tensorflow::Status;
using tflite::Interpreter;

namespace adlik {
namespace serving {
namespace {
void setInputs(Interpreter& interpreter,
               const unordered_map<string_view, int, Hash<string_view>>& inputIndexMap,
               Batch<BatchingMessageTask>& batch) {
  auto numTasks = batch.num_tasks();

  for (auto i = 0; i != numTasks; ++i) {
    const auto& task = batch.task(i);
  }
}
}  // namespace

Status processTensorFlowLiteTask(Interpreter& interpreter,
                                 const unordered_map<string_view, int, Hash<string_view>>& inputIndexMap,
                                 Batch<BatchingMessageTask>& batch) {
  setInputs(interpreter, inputIndexMap, batch);

  throw std::logic_error("Not implemented");
}
}  // namespace serving
}  // namespace adlik
