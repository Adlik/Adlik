// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/libtorch/model/libtorch_util.h"

namespace libtorch_runtime {

const std::map<tensorflow::DataType, torch::Dtype> kTypeConvertMap = {
    {tensorflow::DataType::DT_INT8, torch::kChar},
    {tensorflow::DataType::DT_UINT8, torch::kByte},
    {tensorflow::DataType::DT_INT16, torch::kShort},
    {tensorflow::DataType::DT_INT32, torch::kInt},
    {tensorflow::DataType::DT_INT64, torch::kLong},
    {tensorflow::DataType::DT_FLOAT, torch::kFloat},
    {tensorflow::DataType::DT_DOUBLE, torch::kDouble}};

bool copyData2Buffer(void* content, void* dataPtr, size_t copyByteSize, size_t offsetByteSize) {
  void* bolbBuffer = dataPtr;
  bolbBuffer = (char*)bolbBuffer + offsetByteSize;
  return (content == std::memcpy(content, bolbBuffer, copyByteSize));
}

torch::Dtype ConvertDatatype(const tensorflow::DataType dtype) {
  if (kTypeConvertMap.find(dtype) != kTypeConvertMap.end()) {
    return kTypeConvertMap.at(dtype);
  } else
    return torch::kFloat;
}

bool ConvertDims(const std::vector<int>& shape, adlik::serving::DimsList& dims) {
  dims.Clear();
  for (size_t i = 1; i < shape.size(); ++i) {
    dims.Add(shape[i]);
  }
  return true;
}

}  // namespace libtorch_runtime
