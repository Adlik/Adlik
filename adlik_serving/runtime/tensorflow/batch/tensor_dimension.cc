// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/tensor_dimension.h"

#include "tensorflow/core/framework/register_types.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/types.h"

namespace tensorflow {

void TensorDimension::domerge(const Tensor& tensor) {
  for (auto j = 0; j != tensor.dims(); ++j) {
    auto size = tensor.shape().dim_size(j);
    if (size > sizes[j]) {
      sizes[j] = size;
    }
  }
}

void TensorDimension::init(const Tensor& tensor) {
  sizes = std::vector<int>(tensor.dims(), 0);
}

void TensorDimension::merge(const Tensor& tensor) {
  sizes.empty() ? init(tensor) : domerge(tensor);
}

namespace {
Status fail() {
  return errors::Internal("padding failed");
}

using OneDimPadding = std::pair<int64, int64>;

template <typename T, int NumDims>
struct PaddedTensor {
  using Padding = Eigen::array<OneDimPadding, NumDims>;

  PaddedTensor(const Tensor& input, const std::vector<int>& sizes, Tensor& output) : input(input), output(output) {
    initOutputShape();
    initPadding(sizes);
  }

  Status operator()() const {
    if (isSameShape()) {
      return doCopy();
    } else if (isInputEmpty()) {
      return fail();
    } else {
      return doPadding();
    }
  }

private:
  Status doPadding() const {
    output = Tensor(input.dtype(), outputShape);
    T padValue(input.flat<T>()(0));
    output.tensor<T, NumDims>() = input.tensor<T, NumDims>().pad(padding, padValue);
    return Status::OK();
  }

  Status doCopy() const {
    return output.CopyFrom(input, outputShape) ? Status::OK() : fail();
  }

  bool isSameShape() const {
    return outputShape.num_elements() == input.NumElements();
  }

  bool isInputEmpty() const {
    return input.NumElements() < 1;
  }

  void initOutputShape() {
    for (auto d = 0; d != NumDims; ++d) {
      auto before = padding[d].first;
      auto after = padding[d].second;
      outputShape.AddDim(before + input.dim_size(d) + after);
    }
  }

  void initPadding(const std::vector<int>& sizes) {
    for (decltype(sizes.size()) i = 0; i != sizes.size(); ++i) {
      auto diff = sizes[i] - input.dim_size(i);
      if (i > 0 && diff > 0) {
        padding[i] = {0, diff};
      } else {
        padding[i] = {0, 0};
      }
    }
  }

private:
  const Tensor& input;
  Tensor& output;
  TensorShape outputShape;
  Padding padding;
};

#define PADDING_CASE(n) \
  case n:               \
    return PaddedTensor<T, n>(tensor, sizes, padded)()

template <typename T>
Status doPadding(const Tensor& tensor, const std::vector<int>& sizes, Tensor& padded) {
  switch (tensor.dims()) {
    PADDING_CASE(1);
    PADDING_CASE(2);
    PADDING_CASE(3);
    PADDING_CASE(4);
    PADDING_CASE(5);
    PADDING_CASE(6);
    default:
      return fail();
  }
}

}  // namespace

#define CASE(type)                                 \
  case DataTypeToEnum<type>::value: {              \
    return doPadding<type>(tensor, sizes, padded); \
  }

Status TensorDimension::padding(const Tensor& tensor, Tensor& padded) const {
  switch (tensor.dtype()) {
    TF_CALL_ALL_TYPES(CASE);
    TF_CALL_QUANTIZED_TYPES(CASE);
    TF_CALL_quint16(CASE);
    TF_CALL_qint16(CASE);
    default:
      return errors::InvalidArgument("Unsupported type");
  }
}

}  // namespace tensorflow
