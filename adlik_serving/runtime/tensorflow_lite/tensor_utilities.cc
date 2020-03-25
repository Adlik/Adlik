// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"

using absl::string_view;
using adlik::serving::tensor_tools::useTfLiteTensorTools;
using tensorflow::DataType;

DataType adlik::serving::tfLiteTypeToTfType(TfLiteType tfLiteType) {
  return useTfLiteTensorTools(tfLiteType, [](auto tools) { return decltype(tools)::tfDataType; });
}

string_view adlik::serving::getTFDataTypeName(DataType dataType) {
  switch (dataType) {
    case DataType::DT_FLOAT:
      return "float32";
    case DataType::DT_INT32:
      return "int32";
    case DataType::DT_UINT8:
      return "uint8";
    case DataType::DT_INT64:
      return "int64";
    case DataType::DT_STRING:
      return "string";
    case DataType::DT_BOOL:
      return "bool";
    case DataType::DT_INT16:
      return "int16";
    case DataType::DT_COMPLEX64:
      return "complex64";
    case DataType::DT_INT8:
      return "int8";
    case DataType::DT_HALF:
      return "float16";
    default:
      throw std::invalid_argument("Unsupported type");
  }
}
