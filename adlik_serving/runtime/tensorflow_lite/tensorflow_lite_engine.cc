// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_engine.h"

using tensorflow::Status;
using tflite::Interpreter;

namespace adlik {
namespace serving {
Status processTensorFlowLiteTask(Interpreter& interpreter, Batch<BatchingMessageTask>& batch) {
  throw std::logic_error("Not implemented");
}
}  // namespace serving
}  // namespace adlik
