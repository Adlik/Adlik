// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_ENGINE_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_ENGINE_H

#include "absl/hash/hash.h"
#include "adlik_serving/runtime/batching/batching_message_task.h"
#include "tensorflow/lite/interpreter.h"

namespace adlik {
namespace serving {
tensorflow::Status processTensorFlowLiteTask(
    tflite::Interpreter& interpreter,
    const std::unordered_map<absl::string_view, int, absl::Hash<absl::string_view>>& inputIndexMap,
    Batch<BatchingMessageTask>& batch);
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_ENGINE_H
