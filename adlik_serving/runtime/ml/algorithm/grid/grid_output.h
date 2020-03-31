// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_OUTPUT_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_OUTPUT_H

#include <vector>

#include "adlik_serving/runtime/ml/algorithm/grid/cell.h"
#include "adlik_serving/runtime/ml/algorithm/grid/types.h"

namespace ml_runtime {

struct GridInput;
struct Neighbors;

struct GridOutput {
  GridOutput();
  GridOutput(const Neighbors& neighbors,
             const Rsrp serverRSRP_core,
             const Rsrp neighRSRP_core1,
             const Rsrp neighRSRP_core2);

  void update(const GridInput&);
  void arrangeStats();

  std::vector<std::string> toString() const;

  static std::vector<std::string> fieldNames();

private:
  Rsrp serverRSRP_core;
  Rsrp serverRSRP_max;
  Rsrp serverRSRP_min;

  Cell neighbor1;
  Rsrp neighRSRP_core1;
  Rsrp neighRSRP_max1;
  Rsrp neighRSRP_min1;

  Cell neighbor2;
  Rsrp neighRSRP_core2;
  Rsrp neighRSRP_max2;
  Rsrp neighRSRP_min2;

  size_t neighbor_num;
  struct Statistics {
    Cell neighbor;
    unsigned int event_1 = 0;
    unsigned int event_2 = 0;
    unsigned int event_3 = 0;
  };
  std::vector<Statistics> stats;
};

}  // namespace ml_runtime

#endif
