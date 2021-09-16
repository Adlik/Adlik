// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/paddle/model/paddle_util.h"

namespace paddle_runtime {

size_t GetDataTypeSize(const tensorflow::DataType dtype) {
  switch (dtype) {
    case tensorflow::DataType::DT_BOOL:
      return 1;
    case tensorflow::DataType::DT_INT8:
      return 1;
    case tensorflow::DataType::DT_UINT8:
      return 1;
    case tensorflow::DataType::DT_INT16:
      return 2;
    case tensorflow::DataType::DT_INT32:
      return 4;
    case tensorflow::DataType::DT_INT64:
      return 8;
    case tensorflow::DataType::DT_HALF:
      return 2;
    case tensorflow::DataType::DT_FLOAT:
      return 4;
    case tensorflow::DataType::DT_DOUBLE:
      return 8;
    case tensorflow::DataType::DT_COMPLEX64:
      return 8;
    case tensorflow::DataType::DT_COMPLEX128:
      return 16;
    default:
      return 0;
  }
}

paddle::DataType ConvertToOpenVinoDataType(const tensorflow::DataType dtype) {
  switch (dtype) {
    case tensorflow::DataType::DT_BOOL:
      return paddle::DataType::BOOL;
    case tensorflow::DataType::DT_INT8:
      return paddle::DataType::INT8;
    case tensorflow::DataType::DT_UINT8:
      return paddle::DataType::UINT8;
    case tensorflow::DataType::DT_INT16:
      return paddle::DataType::INT16;
    case tensorflow::DataType::DT_INT32:
      return paddle::DataType::INT32;
    case tensorflow::DataType::DT_INT64:
      return paddle::DataType::INT64;
    case tensorflow::DataType::DT_HALF:
      return paddle::DataType::FLOAT16;
    case tensorflow::DataType::DT_FLOAT:
      return paddle::DataType::FLOAT32;
    case tensorflow::DataType::DT_DOUBLE:
      return paddle::DataType::FLOAT64;
    case tensorflow::DataType::DT_COMPLEX64:
      return paddle::DataType::COMPLEX64;
    case tensorflow::DataType::DT_COMPLEX128:
      return paddle::DataType::COMPLEX128;
    default:
      return paddle::DataType::FLOAT32;
  }
}

// Some data type not supported by paddle inference yet
void CopyFromCpu(std::unique_ptr<paddle_infer::Tensor> tensor, void* buffer, const tensorflow::DataType dtype) {
  switch (dtype) {
    // case tensorflow::DataType::DT_BOOL:
    //   tensor->CopyFromCpu<bool>((bool *)buffer);
    //   return;
    case tensorflow::DataType::DT_INT8:
      tensor->CopyFromCpu<int8_t>((int8_t*)buffer);
      return;
    case tensorflow::DataType::DT_UINT8:
      tensor->CopyFromCpu<uint8_t>((uint8_t*)buffer);
      return;
    // case tensorflow::DataType::DT_INT16:
    //   tensor->CopyFromCpu<int16_t>((int16_t *)buffer);
    //   return;
    case tensorflow::DataType::DT_INT32:
      tensor->CopyFromCpu<int>((int*)buffer);
      return;
    case tensorflow::DataType::DT_INT64:
      tensor->CopyFromCpu<int64_t>((int64_t*)buffer);
      return;
    // case tensorflow::DataType::DT_HALF:
    //   tensor->CopyFromCpu<paddle::float16>((paddle::float16 *)buffer);
    //   return;
    case tensorflow::DataType::DT_FLOAT:
      tensor->CopyFromCpu<float>((float*)buffer);
      return;
    // case tensorflow::DataType::DT_DOUBLE:
    //   tensor->CopyFromCpu<double>((double *)buffer);
    //   return;
    // case tensorflow::DataType::DT_COMPLEX64:
    //   tensor->CopyFromCpu<paddle::platform::complex64>((paddle::platform::complex64 *)buffer);
    //   return;
    // case tensorflow::DataType::DT_COMPLEX128:
    //   tensor->CopyFromCpu<paddle::platform::complex128>((paddle::platform::complex128 *)buffer);
    //   return;
    default:
      tensor->CopyFromCpu<float>((float*)buffer);
      return;
  }
}

// Some data type not supported by paddle inference yet
void CopyToCpu(std::unique_ptr<paddle_infer::Tensor> tensor, void* buffer, const tensorflow::DataType dtype) {
  switch (dtype) {
    // case tensorflow::DataType::DT_BOOL:
    //   tensor->CopyToCpu<bool>((bool *)buffer);
    //   return;
    case tensorflow::DataType::DT_INT8:
      tensor->CopyToCpu<int8_t>((int8_t*)buffer);
      return;
    case tensorflow::DataType::DT_UINT8:
      tensor->CopyToCpu<uint8_t>((uint8_t*)buffer);
      return;
    // case tensorflow::DataType::DT_INT16:
    //   tensor->CopyToCpu<int16_t>((int16_t *)buffer);
    //   return;
    case tensorflow::DataType::DT_INT32:
      tensor->CopyToCpu<int>((int*)buffer);
      return;
    case tensorflow::DataType::DT_INT64:
      tensor->CopyToCpu<int64_t>((int64_t*)buffer);
      return;
    // case tensorflow::DataType::DT_HALF:
    //   tensor->CopyToCpu<paddle::float16>((paddle::float16 *)buffer);
    //   return;
    case tensorflow::DataType::DT_FLOAT:
      tensor->CopyToCpu<float>((float*)buffer);
      return;
    // case tensorflow::DataType::DT_DOUBLE:
    //   tensor->CopyToCpu<double>((double *)buffer);
    //   return;
    // case tensorflow::DataType::DT_COMPLEX64:
    //   tensor->CopyToCpu<paddle::platform::complex64>((paddle::platform::complex64 *)buffer);
    //   return;
    // case tensorflow::DataType::DT_COMPLEX128:
    //   tensor->CopyToCpu<paddle::platform::complex128>((paddle::platform::complex128 *)buffer);
    //   return;
    default:
      tensor->CopyToCpu<float>((float*)buffer);
      return;
  }
}

bool ConvertDims(const std::vector<int>& shape, adlik::serving::DimsList& dims) {
  dims.Clear();
  for (size_t i = 1; i < shape.size(); ++i) {
    dims.Add(shape[i]);
  }
  return true;
}

}  // namespace paddle_runtime