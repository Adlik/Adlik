// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/topsinference/model/topsinference_util.h"

namespace topsinference_runtime {

bool ConvertDims(const std::vector<size_t>& shape, adlik::serving::DimsList& dims) {
  dims.Clear();
  for (size_t i = 1; i < shape.size(); ++i) {
    dims.Add(shape[i]);
  }
  return true;
}

}  // namespace topsinference_runtime