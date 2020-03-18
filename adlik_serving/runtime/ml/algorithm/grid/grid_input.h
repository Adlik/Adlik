// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_INPUT_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_INPUT_H

#include <string>
#include <vector>

#include "adlik_serving/runtime/ml/algorithm/grid/cell.h"
#include "adlik_serving/runtime/ml/algorithm/grid/types.h"
#include "cub/base/status_wrapper.h"

namespace ml_runtime {

struct GridInput {
  Rsrp serving_rsrp;

  Cell neighbor1;
  Rsrp neighRSRP_intra1;

  Cell neighbor2;
  Rsrp neighRSRP_intra2;

  Cell neighbor3;
  Event event;
};

using GridInputs = std::vector<GridInput>;

cub::StatusWrapper loadGridInput(const std::string& file_path, GridInputs& inputs);

}  // namespace ml_runtime

#endif
