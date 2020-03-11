// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H

#include "tensorflow/core/framework/tensor.pb.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/lite/interpreter.h"

namespace adlik {
namespace serving {
class InputContext {
  const int tensorIndex;
  size_t bytesWrittenCache = 0;

public:
  explicit InputContext(int tensorIndex);

  tensorflow::Status addInputTensor(tflite::Interpreter& interpreter, const tensorflow::TensorProto& tensorProto);
  void reset() noexcept;
};
};  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H
