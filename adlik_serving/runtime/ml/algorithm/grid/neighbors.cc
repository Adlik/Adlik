// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/grid/neighbors.h"

#include <vector>

#include "adlik_serving/runtime/ml/algorithm/grid/grid_input.h"
#include "cub/string/str_utils.h"

namespace ml_runtime {

Neighbors::Neighbors(const GridInput& i) : neighbor1(i.neighbor1), neighbor2(i.neighbor2) {
  std::vector<std::string> str = {i.neighbor1.toString(), i.neighbor2.toString()};
  key = cub::strutils::join(str, " & ");
}

bool operator==(const Neighbors& lhs, const Neighbors& rhs) {
  return lhs.key == rhs.key;
}

bool operator!=(const Neighbors& lhs, const Neighbors& rhs) {
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Neighbors& obj) {
  os << obj.key;
  return os;
}

}  // namespace ml_runtime
