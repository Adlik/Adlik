// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/openvino/model/openvino_util.h"

OPENVINO_NS_BEGIN

bool copyBuffer2Blob(const void* content, const ov::Tensor& tensor, size_t copyByteSize, size_t offsetByteSize) {
  void* buffer = tensor.data();
  buffer = (char*)buffer + offsetByteSize;
  return (buffer == std::memcpy(buffer, content, copyByteSize));
}

bool copyBlob2Buffer(void* content, const ov::Tensor& tensor, size_t copyByteSize, size_t offsetByteSize) {
  void* buffer = tensor.data();
  buffer = (char*)buffer + offsetByteSize;
  return (content == std::memcpy(content, buffer, copyByteSize));
}

tensorflow::DataType ConvertToTensorflowDatatype(ov::element::Type openvinoType) {
  switch (openvinoType) {
    case ov::element::f32:
      return tensorflow::DT_FLOAT;
    case ov::element::i8:
      return tensorflow::DT_INT8;
    case ov::element::i32:
      return tensorflow::DT_INT32;
    default:
      return tensorflow::DT_INVALID;
  }
}

bool ConvertDims(const ov::Shape& ovshape, adlik::serving::DimsList& dims) {
  dims.Clear();
  for (size_t i = 1; i < ovshape.size(); ++i) {
    dims.Add(ovshape[i]);
  }
  return true;
}

OPENVINO_NS_END
