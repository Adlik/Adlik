// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H6037ADAB_A18F_483C_8C91_DB0412160620
#define H6037ADAB_A18F_483C_8C91_DB0412160620

#include <map>
#include <string>

#include "adlik_serving/runtime/tensorflow/batch/tensor_dimension.h"

namespace tensorflow {

struct TaskInputs;

struct BatchingInputs {
  void addTask(TaskInputs&);
  const TensorDimension* find(const std::string&) const;

private:
  std::map<std::string, TensorDimension> tensors;
};

}  // namespace tensorflow

#endif
