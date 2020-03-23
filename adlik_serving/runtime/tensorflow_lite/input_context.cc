// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/input_context.h"

using adlik::serving::InputContext;
using adlik::serving::tensor_tools::TfLiteTensorWriter;
using tensorflow::Status;
using tensorflow::TensorProto;

InputContext::InputContext(int tensorIndex, TfLiteTensorWriter writer)
    : tensorIndex(tensorIndex), writer(std::move(writer)) {
}

Status InputContext::writeTensorProto(const TensorProto& tensorProto, TfLiteTensor& tfLiteTensor) {
  return this->writer.writeTensorProto(tensorProto, tfLiteTensor);
}

void InputContext::commit(TfLiteTensor& tfLiteTensor) noexcept {
  return this->writer.commit(tfLiteTensor);
}

void InputContext::reset() noexcept {
  return this->writer.reset();
}

InputContext InputContext::create(int tensorIndex, TfLiteType type) {
  return InputContext{tensorIndex, TfLiteTensorWriter::create(type)};
}
