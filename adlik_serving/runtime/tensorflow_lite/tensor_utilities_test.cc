// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"

#include "gtest/gtest.h"

using adlik::serving::tfLiteTypeToTfType;
using tensorflow::DataType;

TEST(AdlikServingRuntimeTensorFlowLiteTensorUtilities, TfLiteTypeToTfType) {
  ASSERT_EQ(tfLiteTypeToTfType(TfLiteType::kTfLiteNoType), DataType::DT_FLOAT);
  ASSERT_EQ(tfLiteTypeToTfType(TfLiteType::kTfLiteFloat32), DataType::DT_FLOAT);
  ASSERT_EQ(tfLiteTypeToTfType(TfLiteType::kTfLiteInt32), DataType::DT_INT32);
  ASSERT_EQ(tfLiteTypeToTfType(TfLiteType::kTfLiteUInt8), DataType::DT_UINT8);
  ASSERT_EQ(tfLiteTypeToTfType(TfLiteType::kTfLiteInt64), DataType::DT_INT64);
  ASSERT_EQ(tfLiteTypeToTfType(TfLiteType::kTfLiteString), DataType::DT_STRING);
  ASSERT_EQ(tfLiteTypeToTfType(TfLiteType::kTfLiteBool), DataType::DT_BOOL);
  ASSERT_EQ(tfLiteTypeToTfType(TfLiteType::kTfLiteInt16), DataType::DT_INT16);
  ASSERT_EQ(tfLiteTypeToTfType(TfLiteType::kTfLiteComplex64), DataType::DT_COMPLEX64);
  ASSERT_EQ(tfLiteTypeToTfType(TfLiteType::kTfLiteInt8), DataType::DT_INT8);
  ASSERT_EQ(tfLiteTypeToTfType(TfLiteType::kTfLiteFloat16), DataType::DT_HALF);
}
