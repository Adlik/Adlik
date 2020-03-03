// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorrt/model/trt_util.h"

#include <cuda_runtime_api.h>

#include <algorithm>

#include "absl/strings/numbers.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/platform/env.h"

namespace tensorrt {

using namespace adlik::serving;

tensorflow::DataType ConvertDatatype(nvinfer1::DataType trt_type) {
  switch (trt_type) {
    case nvinfer1::DataType::kFLOAT:
      return tensorflow::DT_FLOAT;
    case nvinfer1::DataType::kHALF:
      return tensorflow::DT_HALF;
    case nvinfer1::DataType::kINT8:
      return tensorflow::DT_INT8;
    case nvinfer1::DataType::kINT32:
      return tensorflow::DT_INT32;
  }

  return tensorflow::DT_INVALID;
}

bool ConvertDims(const nvinfer1::Dims& model_dims, adlik::serving::DimsList& dims) {
  dims.Clear();
  for (int i = 0; i < model_dims.nbDims; ++i) {
    dims.Add(model_dims.d[i]);
  }
  return true;
}

bool CompareDims(const nvinfer1::Dims& model_dims, const adlik::serving::DimsList& dims) {
  if (model_dims.nbDims != dims.size()) {
    return false;
  }

  for (int i = 0; i < model_dims.nbDims; ++i) {
    if (model_dims.d[i] != dims[i]) {
      return false;
    }
  }
  return true;
}

tensorflow::Status GetModelVersionFromPath(const tensorflow::StringPiece& path, uint32_t* version) {
  auto version_dir = tensorflow::io::Basename(path);

  // Determine the version from the last segment of 'path'
  if (!absl::SimpleAtoi(version_dir, version)) {
    return tensorflow::errors::Internal("unable to determine model version from ", path);
  }

  return tensorflow::Status::OK();
}

tensorflow::Status ValidateModelInput(const ModelInput& io) {
  if (io.name().empty()) {
    return tensorflow::errors::InvalidArgument("model input must specify 'name'");
  }

  if (io.data_type() == tensorflow::DataType::DT_INVALID) {
    return tensorflow::errors::InvalidArgument("model input must specify 'data_type'");
  }

  if (io.dims_size() == 0) {
    return tensorflow::errors::InvalidArgument("model input must specify 'dims'");
  }

  if (((io.format() == ModelInput::FORMAT_NHWC) || (io.format() == ModelInput::FORMAT_NCHW)) && (io.dims_size() != 3)) {
    return tensorflow::errors::InvalidArgument("model input NHWC/NCHW require 3 dims");
  }

  return tensorflow::Status::OK();
}

tensorflow::Status ValidateModelOutput(const ModelOutput& io) {
  if (io.name().empty()) {
    return tensorflow::errors::InvalidArgument("model output must specify 'name'");
  }

  if (io.data_type() == tensorflow::DT_INVALID) {
    return tensorflow::errors::InvalidArgument("model output must specify 'data_type'");
  }

  if (io.dims_size() == 0) {
    return tensorflow::errors::InvalidArgument("model output must specify 'dims'");
  }

  return tensorflow::Status::OK();
}

}  // namespace tensorrt
