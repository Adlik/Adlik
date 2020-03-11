// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/output_context.h"

#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"

using adlik::serving::DimsList;
using adlik::serving::OutputContext;
using std::tuple;
using tensorflow::DataType;

OutputContext::OutputContext(int tensorIndex, std::string name, DataType dataType)
    : tensorIndex(tensorIndex), name(std::move(name)), dataType(dataType) {
}

const std::string& OutputContext::getName() const {
  return this->name;
}

const DimsList& OutputContext::getDimsList(google::protobuf::int64 batchSize, const TfLiteIntArray& sourceDims) {
  auto result = this->dimsListCache;

  result.Clear();  // Keeps capacity.

  result.Add(batchSize);

  for (auto i = 1; i != sourceDims.size; ++i) {
    result.Add(sourceDims.data[i]);
  }

  return result;
}

void reset() noexcept {
  throw std::logic_error("Not implemented");
}

OutputContext OutputContext::fromTfLiteTensor(int tensorIndex, const TfLiteTensor& tfLiteTensor) {
  return OutputContext{
      tensorIndex,
      tfLiteTensor.name,
      tfLiteTypeToTfType(tfLiteTensor.type),
  };
}
