// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/splited_sizes.h"

#include "tensorflow/core/framework/tensor_util.h"

namespace tensorflow {

void SplitedSizes::add(int64 size) {
  sizes.push_back(size);
}

Status SplitedSizes::split(const Tensor& tensor, SplitedTensors& tensors) const {
  auto status = tensor::Split(tensor, sizes, &tensors);
  if (tensors.size() != sizes.size()) {
    return errors::Internal("invalid tensor shape");
  }
  return Status::OK();
}

}  // namespace tensorflow
