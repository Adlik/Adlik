// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/openvino/model/openvino_util.h"

OPENVINO_NS_BEGIN

using namespace InferenceEngine;

bool copyBuffer2Blob(const void* content, Blob::Ptr blobPtr, size_t copyByteSize, size_t offsetByteSize) {
  void* buffer = blobPtr->buffer();
  buffer = (char*)buffer + offsetByteSize;
  return (buffer == std::memcpy(buffer, content, copyByteSize));
}

bool copyBlob2Buffer(void* content, Blob::Ptr blobPtr, size_t copyByteSize, size_t offsetByteSize) {
  void* bolbBuffer = blobPtr->buffer();
  bolbBuffer = (char*)bolbBuffer + offsetByteSize;
  return (content == std::memcpy(content, bolbBuffer, copyByteSize));
}

Blob::Ptr getBlob(Precision precision, SizeVector dims, Layout layout) {
  Blob::Ptr blobPtr = nullptr;

#define MAKE_SHARED_BLOB(precision) \
  make_shared_blob<PrecisionTrait<precision>::value_type>(TensorDesc(precision, dims, layout))

  switch (precision) {
    case Precision::U8:
      blobPtr = MAKE_SHARED_BLOB(Precision::U8);
      break;
    case Precision::U16:
      blobPtr = MAKE_SHARED_BLOB(Precision::U16);
      break;
    case Precision::I8:
      blobPtr = MAKE_SHARED_BLOB(Precision::I8);
      break;
    case Precision::I16:
      blobPtr = MAKE_SHARED_BLOB(Precision::I16);
      break;
    case Precision::I32:
      blobPtr = MAKE_SHARED_BLOB(Precision::I32);
      break;
    case Precision::FP32:
      blobPtr = MAKE_SHARED_BLOB(Precision::FP32);
      break;
    default:
      blobPtr = nullptr;
  }
  if (blobPtr != nullptr) {
    blobPtr->allocate();
  }
  return blobPtr;
}

Layout ConvertToOpenVinoLayout(adlik::serving::ModelInput_Format format) {
  switch (format) {
    case adlik::serving::ModelInput_Format::ModelInput_Format_FORMAT_NONE:
      return Layout::ANY;
    case adlik::serving::ModelInput_Format::ModelInput_Format_FORMAT_NHWC:
      return Layout::NHWC;
    case adlik::serving::ModelInput_Format::ModelInput_Format_FORMAT_NCHW:
      return Layout::NCHW;
    default:
      return Layout::ANY;
  }
}

Precision ConvertToOpenVinoDataType(const tensorflow::DataType dtype) {
  switch (dtype) {
    case tensorflow::DataType::DT_UINT8:
      return Precision::U8;
    case tensorflow::DataType::DT_UINT16:
      return Precision::U16;
    case tensorflow::DataType::DT_INT8:
      return Precision::I8;
    case tensorflow::DataType::DT_INT16:
      return Precision::I16;
    case tensorflow::DataType::DT_INT32:
      return Precision::I32;
    case tensorflow::DataType::DT_FLOAT:
      return Precision::FP32;
    default:
      return Precision::UNSPECIFIED;
  }
}

tensorflow::DataType ConvertToTensorflowDatatype(Precision openvinoType) {
  switch (openvinoType) {
    case Precision::FP32:
      return tensorflow::DT_FLOAT;
    case Precision::I8:
      return tensorflow::DT_INT8;
    case Precision::I32:
      return tensorflow::DT_INT32;
    default:
      return tensorflow::DT_INVALID;
  }
}

bool ConvertDims(const SizeVector& openvinDim, adlik::serving::DimsList& dims) {
  dims.Clear();
  for (size_t i = 1; i < openvinDim.size(); ++i) {
    dims.Add(openvinDim[i]);
  }
  return true;
}

OPENVINO_NS_END
