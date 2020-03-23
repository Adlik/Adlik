// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/output_context.h"

#include <numeric>

#include "adlik_serving/runtime/tensorflow_lite/itertools.h"
#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"
#include "tensorflow/core/lib/core/coding.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/string_util.h"

using absl::Span;
using adlik::serving::DimsList;

using adlik::serving::OutputContext;
using adlik::serving::tensor_tools::TfLiteTensorReader;
using google::protobuf::RepeatedFieldBackInserter;
using tensorflow::DataType;
using tensorflow::Status;
using tensorflow::core::PutVarint32;
using tflite::Interpreter;

OutputContext::OutputContext(int tensorIndex, std::string name, DataType dataType, TfLiteTensorReader reader)
    : name(std::move(name)), tensorIndex(tensorIndex), dataType(dataType), reader(std::move(reader)) {
}

const std::string& OutputContext::getName() const {
  return this->name;
}

const DimsList& OutputContext::calculateDimsList(const TfLiteIntArray& dims) noexcept {
  auto& result = this->dimsListCache;

  this->dimsListCache.Clear();
  this->dimsListCache.Reserve(dims.size - 1);

  std::copy(dims.data + 1, dims.data + dims.size, RepeatedFieldBackInserter(&this->dimsListCache));

  return this->dimsListCache;
}

OutputContext OutputContext::fromTfLiteTensor(int tensorIndex, const TfLiteTensor& tfLiteTensor) {
  return OutputContext{tensorIndex,
                       tfLiteTensor.name,
                       tfLiteTypeToTfType(tfLiteTensor.type),
                       TfLiteTensorReader::create(tfLiteTensor.type)};
}
