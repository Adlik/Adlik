// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/merged_inputs.h"

#include "adlik_serving/runtime/tensorflow/batch/merged_dimensions.h"
#include "adlik_serving/runtime/tensorflow/batch/tensor_dimension.h"
#include "adlik_serving/runtime/tensorflow/model/model_signature.h"

namespace tensorflow {

MergedInputs::MergedInputs(bool shouldPadding, const MergedDimensions& dims)
    : shouldPadding(shouldPadding), dims(dims) {
}

namespace {
Status fail() {
  return errors::Internal("concat tensors failed");
}
}  // namespace

Status MergedInputs::doPadding(const std::string& name, const Tensor& tensor, Tensor& padded) const {
  if (auto dim = dims.find(name)) {
    return dim->padding(tensor, padded);
  } else {
    return fail();
  }
}

namespace {
Status doCopy(const Tensor& tensor, Tensor& padded) {
  padded = tensor;
  return Status::OK();
}
}  // namespace

Status MergedInputs::padding(const std::string& name, const Tensor& tensor, Tensor& padded) const {
  if (shouldPadding) {
    return doPadding(name, tensor, padded);
  } else {
    return doCopy(tensor, padded);
  }
}

Status MergedInputs::append(const std::string& name, const Tensor& tensor, const FinalPadding& final) {
  Tensor padded;
  TF_RETURN_IF_ERROR(padding(name, tensor, padded));
  tensors[name].append(padded, final);
  return Status::OK();
}

Status MergedInputs::concat(const std::string& name, Tensor& concated) const {
  auto found = tensors.find(name);
  if (found != tensors.cend()) {
    return found->second.concat(concated);
  } else {
    return fail();
  }
}

Status MergedInputs::merge(const std::string& name, InputTensors& merged) const {
  Tensor concated;
  if (concat(name, concated).ok()) {
    merged.push_back({name, concated});
    return Status::OK();
  } else {
    return fail();
  }
}

Status MergedInputs::merge(const ModelSignature& signature, InputTensors& merged) const {
  return signature.inputs([&merged, this](auto& name) { return this->merge(name, merged); });
}

}  // namespace tensorflow
