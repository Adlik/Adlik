// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_INPUT_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_INPUT_H

#include <string>
#include <vector>

#include "adlik_serving/runtime/ml/algorithm/grid/types.h"
#include "cub/base/status_wrapper.h"

namespace ml_runtime {

struct GridInput {
  CellId serving_cell;
  Rsrp serving_rsrp;
  CellId intra_neighbor1;
  Rsrp intra_rsrp1;
  CellId intra_neighbor2;
  Rsrp intra_rsrp2;
};

using GridInputs = std::vector<GridInput>;

cub::StatusWrapper loadGridInput(const std::string& file_path, GridInputs& inputs);

}  // namespace ml_runtime

#endif
