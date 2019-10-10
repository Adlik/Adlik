// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H3853A669_6BEE_4F0D_A8D3_4DB838F5BD97
#define H3853A669_6BEE_4F0D_A8D3_4DB838F5BD97

#include "adlik_serving/runtime/tensorflow/batch/splited_tensors.h"

namespace tensorflow {

struct SplitedSizes {
  void add(int64);
  Status split(const Tensor&, SplitedTensors&) const;

private:
  std::vector<int64> sizes;
};

}  // namespace tensorflow

#endif
