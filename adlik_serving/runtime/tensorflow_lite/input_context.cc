// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/input_context.h"

#include <numeric>

#include "absl/types/span.h"
#include "adlik_serving/runtime/tensorflow_lite/itertools.h"
#include "tensorflow/core/lib/core/coding.h"
#include "tensorflow/core/lib/core/errors.h"

using absl::string_view;
using adlik::serving::copyContainer;
using adlik::serving::InputContext;
using adlik::serving::transformContainer;
using adlik::serving::transformContainerWithStaticCast;
using google::protobuf::RepeatedField;
using google::protobuf::RepeatedPtrField;
using std::string;
using std::vector;
using tensorflow::Status;
using tensorflow::TensorProto;
using tensorflow::TensorShapeProto_Dim;
using tensorflow::core::GetVarint32;
using tensorflow::errors::InvalidArgument;
using tflite::DynamicBuffer;
using tflite::Interpreter;

// TODO: Refactor this file.

namespace {
size_t getNumElements(const RepeatedPtrField<TensorShapeProto_Dim>& field) {
  return static_cast<size_t>(
      std::accumulate(field.begin(), field.end(), int64_t{1}, [](auto acc, auto dim) { return acc * dim.size(); }));
}

void copyComplex64Field(const RepeatedField<float>& field, TfLiteComplex64* target) {
  const auto end = field.end();

  for (auto it = field.begin(); it != end; ++it) {
    target->re = *it;
    ++it;
    target->im = *it;
    ++target;
  }
}

void copyComplex64Content(const string& content, TfLiteComplex64* target) {
  struct Complex {
    float real;
    float imaginary;
  };

  union Reader {
    Complex complex;
    char data[sizeof(Complex)];
  } reader;

  const auto end = content.end();

  for (auto it = content.begin(); it != end;) {
    const auto nextIt = it + sizeof(reader.data);

    std::copy(it, nextIt, reader.data);

    target->re = reader.complex.real;
    target->im = reader.complex.imaginary;

    it = nextIt;
    ++target;
  }
}

bool copyStringContent(const string& content,
                       size_t numElements,
                       DynamicBuffer& dynamicBuffer,
                       vector<uint32_t>& sizesCache) {
  // https://github.com/tensorflow/tensorflow/blob/4e5be621165eee7ef509f7c70fb5d89fcfc84c7a/tensorflow/core/platform/tensor_coding.cc#L46.

  sizesCache.resize(numElements);

  auto reader = string_view{content};

  for (auto& size : sizesCache) {
    if (!GetVarint32(&reader, &size)) {
      return false;
    }
  }

  for (const auto size : sizesCache) {
    dynamicBuffer.AddString(reader.data(), size);
    reader.remove_prefix(size);
  }

  return true;
}
}  // namespace

InputContext::InputContext(int tensorIndex) : tensorIndex{tensorIndex} {
}

Status InputContext::addInputTensor(Interpreter& interpreter, const TensorProto& tensorProto) {
  const auto numElements = getNumElements(tensorProto.tensor_shape().dim());

  if (numElements > 0) {
    const auto& tensor = *interpreter.tensor(this->tensorIndex);
    const auto& tensorContent = tensorProto.tensor_content();

    if (tensorContent.empty()) {
      switch (tensor.type) {
        case TfLiteType::kTfLiteNoType:
          if (static_cast<size_t>(tensorProto.float_val().size()) != numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorProto.float_val(), tensor.data.f + this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteFloat32:
          if (static_cast<size_t>(tensorProto.float_val().size()) != numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorProto.float_val(), tensor.data.f + this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteInt32:
          if (static_cast<size_t>(tensorProto.int_val().size()) != numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorProto.int_val(), tensor.data.i32 + this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteUInt8:
          if (static_cast<size_t>(tensorProto.int_val().size()) != numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          transformContainerWithStaticCast(tensorProto.int_val(), tensor.data.uint8 + this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteInt64:
          if (static_cast<size_t>(tensorProto.int64_val().size()) != numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorProto.int64_val(), tensor.data.i64 + this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteString:
          if (static_cast<size_t>(tensorProto.string_val().size()) != numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          for (const auto& value : tensorProto.string_val()) {
            this->dynamicBuffer.AddString(value.data(), value.length());
          }
          break;
        case TfLiteType::kTfLiteBool:
          if (static_cast<size_t>(tensorProto.bool_val().size()) != numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorProto.bool_val(), tensor.data.b + this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteInt16:
          if (static_cast<size_t>(tensorProto.int_val().size()) != numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          transformContainerWithStaticCast(tensorProto.int_val(), tensor.data.i16 + this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteComplex64:
          if (static_cast<size_t>(tensorProto.scomplex_val().size()) * 2 != numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyComplex64Field(tensorProto.scomplex_val(), tensor.data.c64 + this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteInt8:
          if (static_cast<size_t>(tensorProto.int_val().size()) != numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          transformContainerWithStaticCast(tensorProto.int_val(), tensor.data.int8 + this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteFloat16:
          if (static_cast<size_t>(tensorProto.int_val().size()) != numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          transformContainer(tensorProto.int_val(), tensor.data.f16 + this->elementsWritten, [](auto value) {
            TfLiteFloat16 result;

            result.data = static_cast<decltype(result.data)>(value);

            return result;
          });

          this->elementsWritten += numElements;
          break;
        default:
          throw std::logic_error("Unreachable");
      }
    } else {
      switch (tensor.type) {
        case TfLiteType::kTfLiteNoType:
          if (tensorContent.size() != sizeof(*tensor.data.f) * numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorContent, tensor.data.raw + sizeof(*tensor.data.f) * this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteFloat32:
          if (tensorContent.size() != sizeof(*tensor.data.f) * numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorContent, tensor.data.raw + sizeof(*tensor.data.f) * this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteInt32:
          if (tensorContent.size() != sizeof(*tensor.data.f) * numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorContent, tensor.data.raw + sizeof(*tensor.data.i32) * this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteUInt8:
          if (tensorContent.size() != sizeof(*tensor.data.uint8) * numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorContent, tensor.data.raw + sizeof(*tensor.data.uint8) * this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteInt64:
          if (tensorContent.size() != sizeof(*tensor.data.i64) * numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorContent, tensor.data.raw + sizeof(*tensor.data.i64) * this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteString:
          if (!copyStringContent(tensorContent, numElements, this->dynamicBuffer, this->stringSizesCache)) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          break;
        case TfLiteType::kTfLiteBool:
          if (tensorContent.size() != sizeof(*tensor.data.b) * numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorContent, tensor.data.raw + sizeof(*tensor.data.b) * this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteInt16:
          if (tensorContent.size() != sizeof(*tensor.data.i16) * numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorContent, tensor.data.raw + sizeof(*tensor.data.i16) * this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteComplex64:
          if (tensorContent.size() != sizeof(*tensor.data.c64) * numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyComplex64Content(tensorContent, tensor.data.c64);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteInt8:
          if (tensorContent.size() != sizeof(*tensor.data.int8) * numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorContent, tensor.data.raw + sizeof(*tensor.data.int8) * this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        case TfLiteType::kTfLiteFloat16:
          if (tensorContent.size() != sizeof(*tensor.data.f16) * numElements) {
            return InvalidArgument("TensorProto data size does not match its shape");
          }
          copyContainer(tensorContent, tensor.data.raw + sizeof(*tensor.data.f16) * this->elementsWritten);
          this->elementsWritten += numElements;
          break;
        default:
          throw std::logic_error("Unreachable");
      }
    }
  }

  return Status::OK();
}

void InputContext::commit(Interpreter& interpreter) noexcept {
  auto& tensor = *interpreter.tensor(this->tensorIndex);

  switch (tensor.type) {
    case TfLiteType::kTfLiteString:
      this->dynamicBuffer.WriteToTensor(&tensor, nullptr);
      break;
    default:
      break;
  }
}

void InputContext::reset() noexcept {
  this->elementsWritten = 0;
  this->dynamicBuffer = DynamicBuffer{};
}
