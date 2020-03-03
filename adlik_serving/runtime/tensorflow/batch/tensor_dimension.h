// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H1C7D1984_3AD8_4CFA_9E99_7E4D107FE4B9
#define H1C7D1984_3AD8_4CFA_9E99_7E4D107FE4B9

#include <vector>

#include "tensorflow/core/lib/core/status.h"

namespace tensorflow {

struct Tensor;

struct TensorDimension {
  void merge(const Tensor&);
  Status padding(const Tensor&, Tensor&) const;

private:
  void init(const Tensor&);
  void domerge(const Tensor&);

private:
  std::vector<int> sizes;
};

}  // namespace tensorflow

#endif
