// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/input_context.h"

using adlik::serving::InputContext;
using tensorflow::Status;
using tensorflow::TensorProto;
using tflite::Interpreter;

InputContext::InputContext(int tensorIndex) : tensorIndex{tensorIndex} {
  throw std::logic_error("Not implemented");
}

Status InputContext::addInputTensor(Interpreter& interpreter, const TensorProto& tensorProto) {
  throw std::logic_error("Not implemented");
}

void InputContext::reset() noexcept {
  throw std::logic_error("Not implemented");
}
