// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H

#include "tensorflow/core/framework/tensor.pb.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/string_util.h"

namespace adlik {
namespace serving {
class InputContext {
  const int tensorIndex;

  // Encoder states.

  size_t elementsWritten = 0;
  tflite::DynamicBuffer dynamicBuffer;

  // Caches.

  std::vector<uint32_t> stringSizesCache;

public:
  explicit InputContext(int tensorIndex);

  tensorflow::Status addInputTensor(tflite::Interpreter& interpreter, const tensorflow::TensorProto& tensorProto);
  void commit(tflite::Interpreter& interpreter) noexcept;
  void reset() noexcept;
};
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H
