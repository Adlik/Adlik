// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/paddle/model/paddle_util.h"

namespace paddle_runtime {

// Some data type not supported by paddle inference yet
void CopyFromCpu(std::unique_ptr<paddle_infer::Tensor> tensor, const void* buffer, const tensorflow::DataType dtype) {
  switch (dtype) {
    // case tensorflow::DataType::DT_BOOL:
    //   tensor->CopyFromCpu<bool>((const bool *)buffer);
    //   return;
    case tensorflow::DataType::DT_INT8:
      tensor->CopyFromCpu<int8_t>((const int8_t*)buffer);
      return;
    case tensorflow::DataType::DT_UINT8:
      tensor->CopyFromCpu<uint8_t>((const uint8_t*)buffer);
      return;
    // case tensorflow::DataType::DT_INT16:
    //   tensor->CopyFromCpu<int16_t>((const int16_t *)buffer);
    //   return;
    case tensorflow::DataType::DT_INT32:
      tensor->CopyFromCpu<int>((const int*)buffer);
      return;
    case tensorflow::DataType::DT_INT64:
      tensor->CopyFromCpu<int64_t>((const int64_t*)buffer);
      return;
    // case tensorflow::DataType::DT_HALF:
    //   tensor->CopyFromCpu<paddle::float16>((const paddle::float16 *)buffer);
    //   return;
    case tensorflow::DataType::DT_FLOAT:
      tensor->CopyFromCpu<float>((const float*)buffer);
      return;
    // case tensorflow::DataType::DT_DOUBLE:
    //   tensor->CopyFromCpu<double>((const double *)buffer);
    //   return;
    // case tensorflow::DataType::DT_COMPLEX64:
    //   tensor->CopyFromCpu<paddle::platform::complex64>((const paddle::platform::complex64 *)buffer);
    //   return;
    // case tensorflow::DataType::DT_COMPLEX128:
    //   tensor->CopyFromCpu<paddle::platform::complex128>((const paddle::platform::complex128 *)buffer);
    //   return;
    default:
      tensor->CopyFromCpu<float>((const float*)buffer);
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