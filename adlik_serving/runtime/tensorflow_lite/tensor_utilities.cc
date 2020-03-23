// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"

using tensorflow::DataType;

DataType adlik::serving::tfLiteTypeToTfType(TfLiteType tfLiteType) {
  return adlik::serving::tensor_tools::useTfLiteTensorTools(tfLiteType,
                                                            [](auto tools) { return decltype(tools)::tfDataType; });
}

TfLiteType adlik::serving::tfLiteTypeToTfType(DataType dataType) {
  switch (dataType) {
    case DataType::DT_FLOAT:
      return TfLiteType::kTfLiteFloat32;
    case DataType::DT_INT32:
      return TfLiteType::kTfLiteInt32;
    case DataType::DT_UINT8:
      return TfLiteType::kTfLiteUInt8;
    case DataType::DT_INT64:
      return TfLiteType::kTfLiteInt64;
    case DataType::DT_STRING:
      return TfLiteType::kTfLiteString;
    case DataType::DT_BOOL:
      return TfLiteType::kTfLiteBool;
    case DataType::DT_INT16:
      return TfLiteType::kTfLiteInt16;
    case DataType::DT_COMPLEX64:
      return TfLiteType::kTfLiteComplex64;
    case DataType::DT_INT8:
      return TfLiteType::kTfLiteInt8;
    case DataType::DT_HALF:
      return TfLiteType::kTfLiteFloat16;
    default:
      throw std::invalid_argument("Unsupported type");
  }
}
