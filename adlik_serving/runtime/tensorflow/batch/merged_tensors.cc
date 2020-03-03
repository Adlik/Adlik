// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/merged_tensors.h"

#include "adlik_serving/runtime/tensorflow/batch/final_padding.h"
#include "tensorflow/core/framework/tensor_util.h"

namespace tensorflow {

bool MergedTensors::failed(const Tensor& tensor) const {
  return !tensors.empty() && tensors.front().shape() != tensor.shape();
}

Status MergedTensors::append(const Tensor& tensor) {
  if (failed(tensor)) {
    return errors::FailedPrecondition("should the same tensor shape");
  }
  tensors.push_back(tensor);
  return Status::OK();
}

Status MergedTensors::append(const Tensor& tensor, const FinalPadding& final) {
  TF_RETURN_IF_ERROR(append(tensor));
  final.fill(tensor, *this);
  return Status::OK();
}

Status MergedTensors::concat(Tensor& concated) const {
  return tensor::Concat(tensors, &concated);
}

}  // namespace tensorflow
