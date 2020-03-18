// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/grid/cell.h"

#include <vector>

#include "cub/string/str_utils.h"

namespace ml_runtime {

std::string Cell::toString() const {
  std::vector<std::string> strs = {std::to_string(plmn), std::to_string(nb), std::to_string(cell)};
  return cub::strutils::join(strs, "-");
}

bool operator==(const Cell& lhs, const Cell& rhs) {
  return lhs.plmn == rhs.plmn && lhs.nb == rhs.nb && lhs.cell == rhs.cell;
}
bool operator!=(const Cell& lhs, const Cell& rhs) {
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Cell& cell) {
  os << cell.plmn << "-" << cell.nb << "-" << cell.cell;
  return os;
}

}  // namespace ml_runtime
