// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_LIBTORCH_MODEL_LIBTORCH_UTIL_H
#define ADLIK_SERVING_RUNTIME_LIBTORCH_MODEL_LIBTORCH_UTIL_H

#include <map>
#include <vector>

#include "adlik_serving/framework/domain/dims_list.h"
#include "adlik_serving/framework/domain/model_config.pb.h"
#include "tensorflow/core/lib/core/status.h"
#include "torch/csrc/api/include/torch/types.h"

namespace libtorch_runtime {
bool copyData2Buffer(void* content, void* dataPtr, size_t copyByteSize, size_t offsetByteSize);
bool ConvertDims(const std::vector<int>&, adlik::serving::DimsList&);
torch::Dtype ConvertDatatype(const tensorflow::DataType dtype);

}  // namespace libtorch_runtime
#endif
