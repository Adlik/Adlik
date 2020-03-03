// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORRT_MODEL_TRT_UTIL_H_
#define ADLIK_SERVING_RUNTIME_TENSORRT_MODEL_TRT_UTIL_H_

#include <NvInfer.h>

#include <string>

#include "absl/strings/str_join.h"
#include "adlik_serving/framework/domain/dims_list.h"
#include "adlik_serving/framework/domain/model_config.pb.h"
#include "tensorflow/core/lib/core/status.h"

namespace tensorrt {

constexpr char kTensorRTPlanFilename[] = "model.plan";

tensorflow::DataType ConvertDatatype(nvinfer1::DataType trt_type);

bool ConvertDims(const nvinfer1::Dims& model_dims, adlik::serving::DimsList& dims);

bool CompareDims(const nvinfer1::Dims& model_dims, const adlik::serving::DimsList& dims);

template <typename T>
const std::string DimsDebugString(const T& values) {
  std::string str("[");
  auto content = absl::StrJoin(std::begin(values), std::end(values), ",");
  str.append(content);
  str.append("]");
  return str;
}

tensorflow::Status GetModelVersionFromPath(const tensorflow::StringPiece& path, uint32_t* version);

tensorflow::Status ValidateModelInput(const adlik::serving::ModelInput& io);
tensorflow::Status ValidateModelOutput(const adlik::serving::ModelOutput& io);

}  // namespace tensorrt

#endif
