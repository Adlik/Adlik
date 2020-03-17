// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_INPUT_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_INPUT_H

#include <functional>
#include <string>
#include <vector>

#include "adlik_serving/runtime/ml/algorithm/grid/types.h"
#include "cub/base/status_wrapper.h"

namespace ml_runtime {

struct GridInput {
  PLMN serving_plmn;
  NBId serving_nb;
  CellId serving_cell;
  Rsrp serving_rsrp;

  PLMN neighPLMN_intra1;
  NBId neighNB_intra1;
  CellId neighCID_intra1;
  Rsrp neighRSRP_intra1;

  PLMN neighPLMN_intra2;
  NBId neighNB_intra2;
  CellId neighCID_intra2;
  Rsrp neighRSRP_intra2;

  PLMN neighPLMN_intra3;
  NBId neighNB_intra3;
  CellId neighCID_intra3;
  Event event;
};

using GridInputs = std::vector<GridInput>;
using LoadTrigger = std::function<void(size_t, const GridInput&)>;

inline void defaultLoadTrigger(size_t, const GridInput&) {
}

cub::StatusWrapper loadGridInput(const std::string& file_path,
                                 GridInputs& inputs,
                                 LoadTrigger trigger = defaultLoadTrigger);

}  // namespace ml_runtime

#endif
