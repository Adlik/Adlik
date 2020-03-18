// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_NEIGHBORS_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_NEIGHBORS_H

#include <ostream>
#include <string>

#include "adlik_serving/runtime/ml/algorithm/grid/cell.h"

namespace ml_runtime {

struct GridInput;

struct Neighbors {
  Neighbors(const GridInput&);

  friend bool operator==(const Neighbors&, const Neighbors&);
  friend bool operator!=(const Neighbors&, const Neighbors&);
  friend std::ostream& operator<<(std::ostream&, const Neighbors&);

  const Cell neighbor1;
  const Cell neighbor2;
  std::string key;
};

}  // namespace ml_runtime

namespace std {
template <>
struct hash<ml_runtime::Neighbors> {
  std::size_t operator()(ml_runtime::Neighbors const& s) const noexcept {
    return std::hash<std::string>{}(s.key);
  }
};

}  // namespace std

#endif
