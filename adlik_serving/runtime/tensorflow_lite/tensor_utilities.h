// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSOR_UTILITIES_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSOR_UTILITIES_H

#include <numeric>

#include "absl/types/span.h"
#include "absl/types/variant.h"
#include "adlik_serving/runtime/tensorflow_lite/itertools.h"
#include "tensorflow/core/framework/tensor.pb.h"
#include "tensorflow/core/framework/types.pb.h"
#include "tensorflow/core/lib/core/coding.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/lite/c/c_api_internal.h"
#include "tensorflow/lite/string_util.h"

namespace adlik {
namespace serving {
namespace tensor_tools {
struct ElementsWrittenContext {
  size_t elementsWritten = 0;

  void reset() noexcept {
    this->elementsWritten = 0;
  }
};

struct DynamicBufferContext {
  tflite::DynamicBuffer dynamicBuffer;

  void reset() noexcept {
    this->dynamicBuffer = tflite::DynamicBuffer{};
  }
};

template <TfLiteType>
struct TfLiteTensorTools;

template <class T>
class GetFlatTfLiteTensorToolsValueTypeHelper {
  using ValueType0 = typename std::iterator_traits<decltype(
      T::readField(std::declval<tensorflow::TensorProto>()).begin())>::value_type;

public:
  using ValueType =
      typename std::iterator_traits<decltype(T::getTfLiteDataBuffer(std::declval<TfLiteTensor&>()))>::value_type;

  static_assert(std::is_same<ValueType, ValueType0>::value, "");
};

template <class T>
using GetFlatTfLiteTensorToolsValueType = typename GetFlatTfLiteTensorToolsValueTypeHelper<T>::ValueType;

template <class T, tensorflow::DataType TfDataType>
struct FlatTfLiteTensorTools {
  class Writer {
    size_t elementsWritten = 0;

  public:
    tensorflow::Status writeTensorProto(const tensorflow::TensorProto& tensorProto, TfLiteTensor& tfLiteTensor) {
      using ValueType = GetFlatTfLiteTensorToolsValueType<T>;

      const auto& dims = tensorProto.tensor_shape().dim();

      const auto numElements = static_cast<size_t>(
          std::accumulate(dims.begin(), dims.end(), decltype(dims.begin()->size()){1}, [](auto acc, const auto& value) {
            return acc * value.size();
          }));

      const auto& tensorContent = tensorProto.tensor_content();

      if (tensorContent.length() == sizeof(ValueType) * numElements) {
        std::copy(tensorContent.begin(), tensorContent.end(), tfLiteTensor.data.raw);
      } else {
        const auto& field = T::readField(tensorProto);

        if (static_cast<std::size_t>(field.size()) == numElements) {
          std::copy(std::begin(field), std::end(field), T::getTfLiteDataBuffer(tfLiteTensor));
        } else {
          return tensorflow::errors::InvalidArgument("TensorProto content does not match its shape");
        }
      }

      this->elementsWritten += numElements;

      return tensorflow::Status::OK();
    }

    void commit([[maybe_unused]] TfLiteTensor& tfLiteTensor) noexcept {
    }

    void reset() noexcept {
      this->elementsWritten = 0;
    }
  };

  struct Reader {
    void readTensorProto(const TfLiteTensor& tfLiteTensor,
                         size_t firstElement,
                         size_t numElements,
                         std::string& buffer) const {
      using ValueType = GetFlatTfLiteTensorToolsValueType<T>;

      const auto firstByte = tfLiteTensor.data.raw_const + sizeof(ValueType) * firstElement;
      const auto bufferSize = sizeof(ValueType) * numElements;

      buffer.resize(bufferSize);
      std::copy(firstByte, firstByte + bufferSize, &buffer[0]);
    }
  };  // namespace tensor_tools

  static constexpr auto tfDataType = TfDataType;
};  // namespace serving

template <>
struct TfLiteTensorTools<TfLiteType::kTfLiteFloat32>
    : FlatTfLiteTensorTools<TfLiteTensorTools<TfLiteType::kTfLiteFloat32>, tensorflow::DataType::DT_FLOAT> {
  static decltype(auto) readField(const tensorflow::TensorProto& tensorProto) {
    return tensorProto.float_val();
  }

  static auto getTfLiteDataBuffer(TfLiteTensor& tfLiteTensor) {
    return tfLiteTensor.data.f;
  }
};

template <>
struct TfLiteTensorTools<TfLiteType::kTfLiteNoType> : TfLiteTensorTools<TfLiteType::kTfLiteFloat32> {};

template <>
struct TfLiteTensorTools<TfLiteType::kTfLiteInt32>
    : FlatTfLiteTensorTools<TfLiteTensorTools<TfLiteType::kTfLiteInt32>, tensorflow::DataType::DT_INT32> {
  static decltype(auto) readField(const tensorflow::TensorProto& tensorProto) {
    return tensorProto.int_val();
  }

  static auto getTfLiteDataBuffer(TfLiteTensor& tfLiteTensor) {
    return tfLiteTensor.data.i32;
  }
};

template <>
struct TfLiteTensorTools<TfLiteType::kTfLiteUInt8>
    : FlatTfLiteTensorTools<TfLiteTensorTools<TfLiteType::kTfLiteUInt8>, tensorflow::DataType::DT_UINT8> {
  static decltype(auto) readField(const tensorflow::TensorProto& tensorProto) {
    return itertools::make_map(tensorProto.int_val(), [](auto value) { return static_cast<uint8_t>(value); });
  }

  static auto getTfLiteDataBuffer(TfLiteTensor& tfLiteTensor) {
    return tfLiteTensor.data.uint8;
  }
};

template <>
struct TfLiteTensorTools<TfLiteType::kTfLiteInt64>
    : FlatTfLiteTensorTools<TfLiteTensorTools<TfLiteType::kTfLiteInt64>, tensorflow::DataType::DT_INT64> {
  static decltype(auto) readField(const tensorflow::TensorProto& tensorProto) {
    return tensorProto.int64_val();
  }

  static auto getTfLiteDataBuffer(TfLiteTensor& tfLiteTensor) {
    return tfLiteTensor.data.i64;
  }
};

template <>
struct TfLiteTensorTools<TfLiteType::kTfLiteString> {
  class Writer {
    tflite::DynamicBuffer dynamicBuffer;
    std::vector<uint32_t> sizesCache;

    bool addFromTensorContent(absl::string_view reader, size_t numElements) {
      this->sizesCache.resize(numElements);

      for (auto& size : this->sizesCache) {
        if (!tensorflow::core::GetVarint32(&reader, &size)) {
          return false;
        }
      }

      for (const auto size : this->sizesCache) {
        if (reader.length() >= size) {
          this->dynamicBuffer.AddString(reader.data(), size);
          reader.remove_prefix(size);
        } else {
          return false;
        }
      }

      return true;
    }

  public:
    tensorflow::Status writeTensorProto(const tensorflow::TensorProto& tensorProto,
                                        [[maybe_unused]] TfLiteTensor& tfLiteTensor) {
      const auto& dims = tensorProto.tensor_shape().dim();

      const auto numElements = static_cast<size_t>(
          std::accumulate(dims.begin(), dims.end(), decltype(dims.begin()->size()){1}, [](auto acc, const auto& value) {
            return acc * value.size();
          }));

      if (!this->addFromTensorContent(tensorProto.tensor_content(), numElements)) {
        const auto& strings = tensorProto.string_val();

        if (static_cast<size_t>(strings.size()) == numElements) {
          for (const auto& s : strings) {
            this->dynamicBuffer.AddString(s.data(), s.length());
          }
        } else {
          return tensorflow::errors::InvalidArgument("TensorProto content does not match its shape");
        }
      }

      return tensorflow::Status::OK();
    }

    void commit(TfLiteTensor& tfLiteTensor) noexcept {
      this->dynamicBuffer.WriteToTensor(&tfLiteTensor, nullptr);
    }

    void reset() noexcept {
      this->dynamicBuffer = tflite::DynamicBuffer{};
    }
  };

  struct Reader {
    void readTensorProto(const TfLiteTensor& tfLiteTensor,
                         size_t firstElement,
                         size_t numElements,
                         std::string& buffer) const {
      const auto lastElement = firstElement + numElements;

      for (auto i = firstElement; i != lastElement; ++i) {
        tensorflow::core::PutVarint32(&buffer,
                                      static_cast<uint32_t>(tflite::GetString(&tfLiteTensor, static_cast<int>(i)).len));
      }

      for (auto i = firstElement; i != lastElement; ++i) {
        const auto s = tflite::GetString(&tfLiteTensor, static_cast<int>(i));

        buffer.append(s.str, s.len);
      }
    }
  };

  static constexpr auto tfDataType = tensorflow::DT_STRING;
};

template <>
struct TfLiteTensorTools<TfLiteType::kTfLiteBool>
    : FlatTfLiteTensorTools<TfLiteTensorTools<TfLiteType::kTfLiteBool>, tensorflow::DataType::DT_BOOL> {
  static decltype(auto) readField(const tensorflow::TensorProto& tensorProto) {
    return tensorProto.bool_val();
  }

  static auto getTfLiteDataBuffer(TfLiteTensor& tfLiteTensor) {
    return tfLiteTensor.data.b;
  }
};

template <>
struct TfLiteTensorTools<TfLiteType::kTfLiteInt16>
    : FlatTfLiteTensorTools<TfLiteTensorTools<TfLiteType::kTfLiteInt16>, tensorflow::DataType::DT_INT16> {
  static decltype(auto) readField(const tensorflow::TensorProto& tensorProto) {
    return itertools::make_map(tensorProto.int_val(), [](auto value) { return static_cast<int16_t>(value); });
  }

  static auto getTfLiteDataBuffer(TfLiteTensor& tfLiteTensor) {
    return tfLiteTensor.data.i16;
  }
};

template <>
class TfLiteTensorTools<TfLiteType::kTfLiteComplex64>
    : public FlatTfLiteTensorTools<TfLiteTensorTools<TfLiteType::kTfLiteComplex64>,
                                   tensorflow::DataType::DT_COMPLEX64> {
  class Complex64Values {
    class Iterator {
      const float* p;

    public:
      using difference_type = std::iterator_traits<const float*>::difference_type;
      using value_type = TfLiteComplex64;
      using pointer = void;
      using reference = TfLiteComplex64;
      using iterator_category = std::iterator_traits<const float*>::iterator_category;

      explicit Iterator(const float* p) : p{p} {
      }

      bool operator!=(Iterator rhs) const {
        return this->p != rhs.p;
      }

      Iterator& operator++() {
        this->p += 2;

        return *this;
      }

      TfLiteComplex64 operator*() {
        TfLiteComplex64 result;

        result.re = this->p[0];
        result.im = this->p[1];

        return result;
      }

      auto operator-(Iterator rhs) const {
        return (this->p - rhs.p) / 2;
      }
    };

    const google::protobuf::RepeatedField<float>& base;

  public:
    Complex64Values(const google::protobuf::RepeatedField<float>& base) : base{base} {
    }

    Iterator begin() const {
      return Iterator{this->base.begin()};
    }

    Iterator end() const {
      return Iterator{this->base.begin() + this->size() * 2};
    }

    size_t size() const {
      return static_cast<size_t>(this->base.size() / 2);
    }
  };

public:
  static Complex64Values readField(const tensorflow::TensorProto& tensorProto) {
    return Complex64Values{tensorProto.scomplex_val()};
  }

  static auto getTfLiteDataBuffer(TfLiteTensor& tfLiteTensor) {
    return tfLiteTensor.data.c64;
  }

  static constexpr auto tfDataType = tensorflow::DT_COMPLEX64;
};

template <>
struct TfLiteTensorTools<TfLiteType::kTfLiteInt8>
    : FlatTfLiteTensorTools<TfLiteTensorTools<TfLiteType::kTfLiteInt8>, tensorflow::DataType::DT_INT8> {
  static decltype(auto) readField(const tensorflow::TensorProto& tensorProto) {
    return itertools::make_map(tensorProto.int_val(), [](auto value) { return static_cast<int8_t>(value); });
  }

  static auto getTfLiteDataBuffer(TfLiteTensor& tfLiteTensor) {
    return tfLiteTensor.data.int8;
  }
};

template <>
struct TfLiteTensorTools<TfLiteType::kTfLiteFloat16>
    : FlatTfLiteTensorTools<TfLiteTensorTools<TfLiteType::kTfLiteFloat16>, tensorflow::DataType::DT_HALF> {
  static decltype(auto) readField(const tensorflow::TensorProto& tensorProto) {
    return itertools::make_map(tensorProto.half_val(),
                               [](auto value) { return TfLiteFloat16{static_cast<uint16_t>(value)}; });
  }

  static auto getTfLiteDataBuffer(TfLiteTensor& tfLiteTensor) {
    return tfLiteTensor.data.f16;
  }
};

template <class F>
decltype(auto) useTfLiteTensorTools(TfLiteType type, F&& f) {
  switch (type) {
    case TfLiteType::kTfLiteNoType:
      return std::forward<F>(f)(TfLiteTensorTools<TfLiteType::kTfLiteNoType>{});
    case TfLiteType::kTfLiteFloat32:
      return std::forward<F>(f)(TfLiteTensorTools<TfLiteType::kTfLiteFloat32>{});
    case TfLiteType::kTfLiteInt32:
      return std::forward<F>(f)(TfLiteTensorTools<TfLiteType::kTfLiteInt32>{});
    case TfLiteType::kTfLiteUInt8:
      return std::forward<F>(f)(TfLiteTensorTools<TfLiteType::kTfLiteUInt8>{});
    case TfLiteType::kTfLiteInt64:
      return std::forward<F>(f)(TfLiteTensorTools<TfLiteType::kTfLiteInt64>{});
    case TfLiteType::kTfLiteString:
      return std::forward<F>(f)(TfLiteTensorTools<TfLiteType::kTfLiteString>{});
    case TfLiteType::kTfLiteBool:
      return std::forward<F>(f)(TfLiteTensorTools<TfLiteType::kTfLiteBool>{});
    case TfLiteType::kTfLiteInt16:
      return std::forward<F>(f)(TfLiteTensorTools<TfLiteType::kTfLiteInt16>{});
    case TfLiteType::kTfLiteComplex64:
      return std::forward<F>(f)(TfLiteTensorTools<TfLiteType::kTfLiteComplex64>{});
    case TfLiteType::kTfLiteInt8:
      return std::forward<F>(f)(TfLiteTensorTools<TfLiteType::kTfLiteInt8>{});
    case TfLiteType::kTfLiteFloat16:
      return std::forward<F>(f)(TfLiteTensorTools<TfLiteType::kTfLiteFloat16>{});
    default:
      throw std::invalid_argument("Invalid type");
  }
}

class TfLiteTensorWriter {
  absl::variant<TfLiteTensorTools<TfLiteType::kTfLiteNoType>::Writer,
                TfLiteTensorTools<TfLiteType::kTfLiteFloat32>::Writer,
                TfLiteTensorTools<TfLiteType::kTfLiteInt32>::Writer,
                TfLiteTensorTools<TfLiteType::kTfLiteUInt8>::Writer,
                TfLiteTensorTools<TfLiteType::kTfLiteInt64>::Writer,
                TfLiteTensorTools<TfLiteType::kTfLiteString>::Writer,
                TfLiteTensorTools<TfLiteType::kTfLiteBool>::Writer,
                TfLiteTensorTools<TfLiteType::kTfLiteInt16>::Writer,
                TfLiteTensorTools<TfLiteType::kTfLiteComplex64>::Writer,
                TfLiteTensorTools<TfLiteType::kTfLiteInt8>::Writer,
                TfLiteTensorTools<TfLiteType::kTfLiteFloat16>::Writer>
      variants;

  template <class T>
  explicit TfLiteTensorWriter(T&& writer) : variants{std::forward<T>(writer)} {
  }

public:
  tensorflow::Status writeTensorProto(const tensorflow::TensorProto& tensorProto, TfLiteTensor& tfLiteTensor) {
    return absl::visit([&](auto& impl) { return impl.writeTensorProto(tensorProto, tfLiteTensor); }, this->variants);
  }

  void commit(TfLiteTensor& tfLiteTensor) noexcept {
    absl::visit([&](auto& impl) { impl.commit(tfLiteTensor); }, this->variants);
  }

  void reset() noexcept {
    absl::visit([&](auto& impl) { impl.reset(); }, this->variants);
  }

  static TfLiteTensorWriter create(TfLiteType type) {
    return useTfLiteTensorTools(type,
                                [](auto tools) { return TfLiteTensorWriter{typename decltype(tools)::Writer{}}; });
  }
};

class TfLiteTensorReader {
  absl::variant<TfLiteTensorTools<TfLiteType::kTfLiteNoType>::Reader,
                TfLiteTensorTools<TfLiteType::kTfLiteFloat32>::Reader,
                TfLiteTensorTools<TfLiteType::kTfLiteInt32>::Reader,
                TfLiteTensorTools<TfLiteType::kTfLiteUInt8>::Reader,
                TfLiteTensorTools<TfLiteType::kTfLiteInt64>::Reader,
                TfLiteTensorTools<TfLiteType::kTfLiteString>::Reader,
                TfLiteTensorTools<TfLiteType::kTfLiteBool>::Reader,
                TfLiteTensorTools<TfLiteType::kTfLiteInt16>::Reader,
                TfLiteTensorTools<TfLiteType::kTfLiteComplex64>::Reader,
                TfLiteTensorTools<TfLiteType::kTfLiteInt8>::Reader,
                TfLiteTensorTools<TfLiteType::kTfLiteFloat16>::Reader>
      variants;

  template <class T>
  explicit TfLiteTensorReader(T&& reader) : variants{std::forward<T>(reader)} {
  }

public:
  template <class F>
  decltype(auto) visit(F&& f) {
    return absl::visit(std::forward<F>(f), this->variants);
  }

  static TfLiteTensorReader create(TfLiteType type) {
    return useTfLiteTensorTools(type,
                                [](auto tools) { return TfLiteTensorReader{typename decltype(tools)::Reader{}}; });
  }
};
}  // namespace tensor_tools

tensorflow::DataType tfLiteTypeToTfType(TfLiteType tfLiteType);
TfLiteType tfLiteTypeToTfType(tensorflow::DataType dataType);
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSOR_UTILITIES_H
