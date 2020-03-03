// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/splited_outputs.h"

#include "adlik_serving/runtime/tensorflow/batch/splited_sizes.h"

namespace tensorflow {

SplitedOuputs::SplitedOuputs(SplitedSizes& sizes) : sizes(sizes) {
}

Status SplitedOuputs::split(const std::string& name, const Tensor& tensor) {
  if (tensor.dims() == 0) {
    return errors::FailedPrecondition("Batched output tensor has 0 dimensions");
  }
  return sizes.split(tensor, tensors[name]);
}

Status SplitedOuputs::split(const OutputNames& outputs, const OutputTensors& mergeds) {
  for (decltype(outputs.size()) i = 0; i != outputs.size(); ++i) {
    TF_RETURN_IF_ERROR(split(outputs[i], mergeds[i]));
  }
  return Status::OK();
}

const SplitedTensors* SplitedOuputs::find(const std::string& name) const {
  auto found = tensors.find(name);
  return found != tensors.cend() ? &found->second : nullptr;
}

}  // namespace tensorflow
