// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/output_context.h"

#include "tensorflow/lite/interpreter.h"

using adlik::serving::DimsList;
using adlik::serving::OutputContext;
using adlik::serving::tensor_tools::TfLiteTensorReader;
using google::protobuf::RepeatedFieldBackInserter;
using std::string;
using tensorflow::DataType;

OutputContext::OutputContext(int tensorIndex, string name, DataType dataType, TfLiteTensorReader reader)
    : tensorIndex(tensorIndex), name(std::move(name)), dataType(dataType), reader(std::move(reader)) {
}

const string& OutputContext::getName() const {
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
