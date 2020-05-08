// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HDD9F7A07_3542_443C_8EBD_A67F323D731E
#define HDD9F7A07_3542_443C_8EBD_A67F323D731E

#include <map>
#include <string>

#include "adlik_serving/runtime/tensorflow/batch/tensor_dimension.h"

namespace tensorflow {

struct Tensor;

struct MergedDimensions {
  void append(const std::string&, const Tensor&);
  const TensorDimension* find(const std::string&) const;

private:
  std::map<std::string, TensorDimension> dims;
};

}  // namespace tensorflow

#endif
