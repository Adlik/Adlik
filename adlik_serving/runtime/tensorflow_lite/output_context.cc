// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/output_context.h"

#include <numeric>

#include "adlik_serving/runtime/tensorflow_lite/itertools.h"
#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"
#include "tensorflow/core/lib/core/coding.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/lite/string_util.h"

using absl::Span;
using adlik::serving::DimsList;

using adlik::serving::OutputContext;
using google::protobuf::RepeatedFieldBackInserter;
using tensorflow::DataType;
using tensorflow::Status;
using tensorflow::core::PutVarint32;
using tflite::Interpreter;

namespace {
auto getSampleElements(const TfLiteIntArray& dims) {
  return std::accumulate(dims.data + 1, dims.data + dims.size, 1, std::multiplies<int>{});
}
}  // namespace

OutputContext::OutputContext(int tensorIndex, std::string name, DataType dataType)
    : tensorIndex(tensorIndex), name(std::move(name)), dataType(dataType) {
}

const std::string& OutputContext::getName() const {
  return this->name;
}

const DimsList& OutputContext::getDimsList(Span<const int> dims) {
  auto& result = this->dimsListCache;

  result.Clear();

  std::copy(dims.begin(), dims.end(), RepeatedFieldBackInserter(&result));

  return result;
}

Status OutputContext::readBatch(const Interpreter& interpreter, size_t batchSize, std::string& target) {
  const auto& tensor = *interpreter.tensor(this->tensorIndex);
  const auto sampleElements = getSampleElements(*tensor.dims);

  const auto copyBuffer = [&, this](size_t elementSize) {
    const auto sampleSize = elementSize * sampleElements;
    const auto batchByteSize = sampleSize * batchSize;

    target.resize(batchByteSize);

    const auto first = tensor.data.raw_const + sampleSize * this->elementsRead;

    std::copy(first, first + batchByteSize, std::back_inserter(target));
  };

  switch (tensor.type) {
    case TfLiteType::kTfLiteNoType:
      copyBuffer(sizeof(*tensor.data.f));
      break;
    case TfLiteType::kTfLiteFloat32:
      copyBuffer(sizeof(*tensor.data.f));
      break;
    case TfLiteType::kTfLiteInt32:
      copyBuffer(sizeof(*tensor.data.i32));
      break;
    case TfLiteType::kTfLiteUInt8:
      copyBuffer(sizeof(*tensor.data.uint8));
      break;
    case TfLiteType::kTfLiteInt64:
      copyBuffer(sizeof(*tensor.data.i64));
      break;
    case TfLiteType::kTfLiteString: {
      const auto first = sampleElements * this->elementsRead;
      const auto last = first + sampleElements * batchSize;

      for (auto i = first; i != last; ++i) {
        PutVarint32(&target, tflite::GetString(&tensor, i).len);
      }

      for (auto i = first; i != last; ++i) {
        const auto s = tflite::GetString(&tensor, i);

        target.append(s.str, s.len);
      }

      break;
    }
    case TfLiteType::kTfLiteBool:
      copyBuffer(sizeof(*tensor.data.b));
      break;
    case TfLiteType::kTfLiteInt16:
      copyBuffer(sizeof(*tensor.data.i16));
      break;
    case TfLiteType::kTfLiteComplex64:
      static_assert(sizeof(TfLiteComplex64) == 8);
      static_assert(offsetof(TfLiteComplex64, re) == 0);
      static_assert(offsetof(TfLiteComplex64, im) == 4);
      copyBuffer(sizeof(*tensor.data.c64));
      break;
    case TfLiteType::kTfLiteInt8:
      copyBuffer(sizeof(*tensor.data.int8));
      break;
    case TfLiteType::kTfLiteFloat16:
      static_assert(sizeof(TfLiteFloat16) == 2);
      static_assert(offsetof(TfLiteFloat16, data) == 0);
      copyBuffer(sizeof(*tensor.data.f16));
      break;
    default:
      throw std::logic_error("Unreachable");
  }

  this->elementsRead += batchSize;
}

void OutputContext::reset() noexcept {
  this->elementsRead = 0;
}

OutputContext OutputContext::fromTfLiteTensor(int tensorIndex, const TfLiteTensor& tfLiteTensor) {
  return OutputContext{
      tensorIndex,
      tfLiteTensor.name,
      tfLiteTypeToTfType(tfLiteTensor.type),
  };
}
