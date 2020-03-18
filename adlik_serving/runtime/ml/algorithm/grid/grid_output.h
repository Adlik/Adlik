// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_OUTPUT_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_OUTPUT_H

#include <memory>
#include <vector>

#include "adlik_serving/runtime/ml/algorithm/grid/cell.h"
#include "adlik_serving/runtime/ml/algorithm/grid/types.h"
#include "cub/base/status_wrapper.h"

namespace ml_runtime {

struct GridResult {
  static size_t maxNeighborNum();

  // unsigned long grid_id;

  Rsrp serverRSRP_core;
  Rsrp serverRSRP_max = MIN_RSRP;
  Rsrp serverRSRP_min = MAX_RSRP;

  Cell neighbor1;
  Rsrp neighRSRP_core1;
  Rsrp neighRSRP_max1 = MIN_RSRP;
  Rsrp neighRSRP_min1 = MAX_RSRP;

  Cell neighbor2;
  Rsrp neighRSRP_core2;
  Rsrp neighRSRP_max2 = MIN_RSRP;
  Rsrp neighRSRP_min2 = MAX_RSRP;

  size_t neighbor_num = 0;  // max 10
  struct Statistics {
    Cell neighbor;
    unsigned int event_1 = 0;
    unsigned int event_2 = 0;
    unsigned int event_3 = 0;
  };
  std::vector<Statistics> stats;
};

struct GridCsvSaver {
  static cub::StatusWrapper create(const std::string&, std::unique_ptr<GridCsvSaver>*);

  virtual bool save(const GridResult&) = 0;
  bool save(const std::vector<GridResult>&);

  virtual ~GridCsvSaver() {
  }
};

}  // namespace ml_runtime

#endif
