// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/grid/rsrp_grid.h"

#include "adlik_serving/runtime/ml/algorithm/grid/grid_input.h"
#include "cub/string/str_utils.h"

namespace ml_runtime {

namespace {

#define POSITIVE_RSRP(rsrp) (rsrp + 140)

Rsrp lambda(Rsrp rsrp, Rsrp size) {
  return (Rsrp)((POSITIVE_RSRP(rsrp)) / size);
}

}  // namespace

RsrpGrid::RsrpGrid(Rsrp size, const GridInput& i) : size(size) {
  std::vector<std::string> str = {std::to_string(lambda(i.serving_rsrp, size)),
                                  std::to_string(lambda(i.neighRSRP_intra1, size)),
                                  std::to_string(lambda(i.neighRSRP_intra2, size))};
  key = cub::strutils::join(str, ",");
}

bool operator==(const RsrpGrid& lhs, const RsrpGrid& rhs) {
  return lhs.key == rhs.key;
}

bool operator!=(const RsrpGrid& lhs, const RsrpGrid& rhs) {
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const RsrpGrid& obj) {
  os << obj.key;
  return os;
}

}  // namespace ml_runtime
