// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"

using tensorflow::DataType;

DataType adlik::serving::tfLiteTypeToTfType(TfLiteType tfLiteType) {
  switch (tfLiteType) {
    case TfLiteType::kTfLiteNoType:
      // https://github.com/tensorflow/tensorflow/blob/4601949937145e66df37483c460ba9b7bfdfa680/tensorflow/lite/delegates/flex/util.cc#L60
      return DataType::DT_FLOAT;
    case TfLiteType::kTfLiteFloat32:
      return DataType::DT_FLOAT;
    case TfLiteType::kTfLiteInt32:
      return DataType::DT_INT32;
    case TfLiteType::kTfLiteUInt8:
      return DataType::DT_UINT8;
    case TfLiteType::kTfLiteInt64:
      return DataType::DT_INT64;
    case TfLiteType::kTfLiteString:
      return DataType::DT_STRING;
    case TfLiteType::kTfLiteBool:
      return DataType::DT_BOOL;
    case TfLiteType::kTfLiteInt16:
      return DataType::DT_INT16;
    case TfLiteType::kTfLiteComplex64:
      return DataType::DT_COMPLEX64;
    case TfLiteType::kTfLiteInt8:
      return DataType::DT_INT8;
    case TfLiteType::kTfLiteFloat16:
      return DataType::DT_HALF;
    default:
      throw std::logic_error("Unreachable");
  }
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
