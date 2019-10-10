// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/merged_dimensions.h"

namespace tensorflow {

void MergedDimensions::append(const std::string& name, const Tensor& tensor) {
  dims[name].merge(tensor);
}

const TensorDimension* MergedDimensions::find(const std::string& name) const {
  auto found = dims.find(name);
  return found != dims.cend() ? &found->second : nullptr;
}

}  // namespace tensorflow
