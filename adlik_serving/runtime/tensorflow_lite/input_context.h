// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H

#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"

namespace adlik {
namespace serving {
class InputContext {
  InputContext(int tensorIndex, tensor_tools::TfLiteTensorWriter writer)
      : tensorIndex(tensorIndex), writer(std::move(writer)) {
  }

public:
  const int tensorIndex;
  tensor_tools::TfLiteTensorWriter writer;

  static InputContext create(int tensorIndex, TfLiteType type) {
    return InputContext{tensorIndex, tensor_tools::TfLiteTensorWriter::create(type)};
  }
};
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H
