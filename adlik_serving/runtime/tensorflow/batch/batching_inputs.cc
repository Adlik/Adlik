// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/batching_inputs.h"

#include "adlik_serving/runtime/tensorflow/batch/inferential_task.h"
#include "tensorflow/core/framework/tensor.h"

namespace tensorflow {

const TensorDimension* BatchingInputs::find(const std::string& name) const {
  auto found = tensors.find(name);
  return found != tensors.cend() ? &found->second : nullptr;
}

void BatchingInputs::addTask(TaskInputs& task) {
  task.inputs([this](auto& name, auto& tensor) { tensors[name].merge(tensor); });
}

}  // namespace tensorflow
