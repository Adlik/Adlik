// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_OUTPUT_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_OUTPUT_H

#include <unordered_map>
#include <utility>
#include <vector>

#include "adlik_serving/runtime/ml/algorithm/grid/types.h"

namespace ml_runtime {

using RsrpInterval = std::pair<Rsrp, Rsrp>;
struct GridResult {
  unsigned int grid_id;

  CellId serving_cell;
  RsrpInterval serving_rsrp;
  CellId intra_neighbor1;
  RsrpInterval intra_rsrp1;
  CellId intra_neighbor2;
  RsrpInterval intra_rsrp2;

  // todo: centers information

  // statistics information
  using Events = std::vector<unsigned int>;
  std::unordered_map<CellId, Events> statistics;
};

}  // namespace ml_runtime

#endif
