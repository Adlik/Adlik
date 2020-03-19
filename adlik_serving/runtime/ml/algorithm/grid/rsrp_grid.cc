// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/grid/rsrp_grid.h"

#include "adlik_serving/runtime/ml/algorithm/grid/grid_input.h"
#include "cub/string/str_utils.h"

namespace ml_runtime {

namespace {

#define POSITIVE_RSRP(rsrp) (rsrp + 140)

}  // namespace

RsrpGrid::RsrpGrid(Rsrp size, const GridInput& i) {
  auto func = [&](Rsrp rsrp) { return std::to_string((POSITIVE_RSRP(rsrp)) / size); };

  std::vector<std::string> str = {func(i.serving_rsrp), func(i.neighRSRP_intra1), func(i.neighRSRP_intra2)};
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
