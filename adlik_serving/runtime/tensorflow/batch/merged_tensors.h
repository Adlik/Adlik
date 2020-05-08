// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HA18C6EAA_8657_4863_8732_8C9098A438C6
#define HA18C6EAA_8657_4863_8732_8C9098A438C6

#include <vector>

#include "tensorflow/core/framework/tensor.h"

namespace tensorflow {

struct FinalPadding;

struct MergedTensors {
  Status append(const Tensor&);
  Status append(const Tensor&, const FinalPadding& final);

  Status concat(Tensor&) const;

private:
  bool failed(const Tensor&) const;

private:
  std::vector<Tensor> tensors;
};

}  // namespace tensorflow

#endif
