// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_RSRP_GRID_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_RSRP_GRID_H

#include <ostream>

#include "adlik_serving/runtime/ml/algorithm/grid/types.h"

namespace ml_runtime {

struct GridInput;

struct RsrpGrid {
  RsrpGrid(Rsrp size, const GridInput&);

  friend bool operator==(const RsrpGrid&, const RsrpGrid&);
  friend bool operator!=(const RsrpGrid&, const RsrpGrid&);
  friend std::ostream& operator<<(std::ostream&, const RsrpGrid&);

  const Rsrp size;
  std::string key;
};

}  // namespace ml_runtime

namespace std {

template <>
struct hash<ml_runtime::RsrpGrid> {
  std::size_t operator()(ml_runtime::RsrpGrid const& s) const noexcept {
    return std::hash<std::string>{}(s.key);
  }
};

}  // namespace std

#endif
