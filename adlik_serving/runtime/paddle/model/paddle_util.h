// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_PADDLE_MODEL_PADDLE_UTIL_H
#define ADLIK_SERVING_RUNTIME_PADDLE_MODEL_PADDLE_UTIL_H

#include <vector>

#include "adlik_serving/framework/domain/dims_list.h"
#include "adlik_serving/framework/domain/model_config.pb.h"
#include "paddle/include/paddle_tensor.h"
#include "tensorflow/core/lib/core/status.h"

namespace paddle_runtime {

void CopyFromCpu(std::unique_ptr<paddle_infer::Tensor> tensor, const void* buffer, const tensorflow::DataType dtype);
void CopyToCpu(std::unique_ptr<paddle_infer::Tensor> tensor, void* buffer, const tensorflow::DataType dtype);
bool ConvertDims(const std::vector<int>&, adlik::serving::DimsList&);

}  // namespace paddle_runtime
#endif
