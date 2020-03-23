// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H

#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"

namespace adlik {
namespace serving {
class InputContext {
public:
  const int tensorIndex;

private:
  tensor_tools::TfLiteTensorWriter writer;

  InputContext(int tensorIndex, tensor_tools::TfLiteTensorWriter writer);

public:
  tensorflow::Status writeTensorProto(const tensorflow::TensorProto& tensorProto, TfLiteTensor& tfLiteTensor);
  void commit(TfLiteTensor& tfLiteTensor) noexcept;
  void reset() noexcept;

  static InputContext create(int tensorIndex, TfLiteType type);
};
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_INPUT_CONTEXT_H
