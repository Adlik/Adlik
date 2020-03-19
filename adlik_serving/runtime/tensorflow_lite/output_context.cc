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
size_t getSampleElements(const TfLiteIntArray& dims) {
  return static_cast<std::size_t>(std::accumulate(dims.data + 1, dims.data + dims.size, 1, std::multiplies<int>{}));
}
}  // namespace

OutputContext::OutputContext(int tensorIndex, std::string name, DataType dataType)
    : name(std::move(name)), tensorIndex(tensorIndex), dataType(dataType) {
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

void OutputContext::readBatch(const TfLiteTensor& tfLiteTensor,
                              size_t firstElement,
                              size_t numElements,
                              std::string& target) {
  const auto copyBuffer = [&, this](size_t elementSize) {
    const auto batchByteSize = elementSize * numElements;
    const auto first = tfLiteTensor.data.raw_const + elementSize * firstElement;
    const auto last = first + batchByteSize;

    target.resize(batchByteSize);
    std::copy(first, last, &target[0]);
  };

  switch (tfLiteTensor.type) {
    case TfLiteType::kTfLiteNoType:
      copyBuffer(sizeof(*tfLiteTensor.data.f));
      break;
    case TfLiteType::kTfLiteFloat32:
      copyBuffer(sizeof(*tfLiteTensor.data.f));
      break;
    case TfLiteType::kTfLiteInt32:
      copyBuffer(sizeof(*tfLiteTensor.data.i32));
      break;
    case TfLiteType::kTfLiteUInt8:
      copyBuffer(sizeof(*tfLiteTensor.data.uint8));
      break;
    case TfLiteType::kTfLiteInt64:
      copyBuffer(sizeof(*tfLiteTensor.data.i64));
      break;
    case TfLiteType::kTfLiteString: {
      const auto lastElement = firstElement + numElements;

      for (auto i = firstElement; i != lastElement; ++i) {
        PutVarint32(&target, tflite::GetString(&tfLiteTensor, i).len);
      }

      for (auto i = firstElement; i != lastElement; ++i) {
        const auto s = tflite::GetString(&tfLiteTensor, i);

        target.append(s.str, s.len);
      }

      break;
    }
    case TfLiteType::kTfLiteBool:
      copyBuffer(sizeof(*tfLiteTensor.data.b));
      break;
    case TfLiteType::kTfLiteInt16:
      copyBuffer(sizeof(*tfLiteTensor.data.i16));
      break;
    case TfLiteType::kTfLiteComplex64:
      static_assert(sizeof(TfLiteComplex64) == 8);
      static_assert(offsetof(TfLiteComplex64, re) == 0);
      static_assert(offsetof(TfLiteComplex64, im) == 4);
      copyBuffer(sizeof(*tfLiteTensor.data.c64));
      break;
    case TfLiteType::kTfLiteInt8:
      copyBuffer(sizeof(*tfLiteTensor.data.int8));
      break;
    case TfLiteType::kTfLiteFloat16:
      static_assert(sizeof(TfLiteFloat16) == 2);
      static_assert(offsetof(TfLiteFloat16, data) == 0);
      copyBuffer(sizeof(*tfLiteTensor.data.f16));
      break;
    default:
      throw std::logic_error("Unreachable");
  }
}

OutputContext OutputContext::fromTfLiteTensor(int tensorIndex, const TfLiteTensor& tfLiteTensor) {
  return OutputContext{
      tensorIndex,
      tfLiteTensor.name,
      tfLiteTypeToTfType(tfLiteTensor.type),
  };
}
