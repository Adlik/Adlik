// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_ENGINE_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_ENGINE_H

#include "absl/hash/hash.h"
#include "absl/types/span.h"
#include "adlik_serving/runtime/batching/batching_message_task.h"
#include "adlik_serving/runtime/tensorflow_lite/input_context.h"
#include "adlik_serving/runtime/tensorflow_lite/output_context.h"
#include "tensorflow/lite/interpreter.h"

namespace adlik {
namespace serving {
tensorflow::Status processTensorFlowLiteTask(
    tflite::Interpreter& interpreter,
    std::unordered_map<absl::string_view, InputContext, absl::Hash<absl::string_view>>& inputContextMap,
    absl::Span<OutputContext> outputContexts,
    Batch<BatchingMessageTask>& batch);
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_ENGINE_H
