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

  template <typename F>
  void accept(F&& f) const {
    std::forward<F>(f)(this->serverRSRP_core);
    std::forward<F>(f)(this->serverRSRP_max);
    std::forward<F>(f)(this->serverRSRP_min);

    std::forward<F>(f)(this->neighbor1.plmn);
    std::forward<F>(f)(this->neighbor1.nb);
    std::forward<F>(f)(this->neighbor1.cell);
    std::forward<F>(f)(this->neighRSRP_core1);
    std::forward<F>(f)(this->neighRSRP_max1);
    std::forward<F>(f)(this->neighRSRP_min1);

    std::forward<F>(f)(this->neighbor2.plmn);
    std::forward<F>(f)(this->neighbor2.nb);
    std::forward<F>(f)(this->neighbor2.cell);
    std::forward<F>(f)(this->neighRSRP_core2);
    std::forward<F>(f)(this->neighRSRP_max2);
    std::forward<F>(f)(this->neighRSRP_min2);

    std::forward<F>(f)(this->neighbor_num);
    for (size_t i = 0; i < neighbor_num && i < this->stats.size(); ++i) {
      std::forward<F>(f)(this->stats[i].neighbor.plmn);
      std::forward<F>(f)(this->stats[i].neighbor.nb);
      std::forward<F>(f)(this->stats[i].neighbor.cell);
      std::forward<F>(f)(this->stats[i].event_1);
      std::forward<F>(f)(this->stats[i].event_2);
      std::forward<F>(f)(this->stats[i].event_3);
    }
  }

  static std::vector<std::string> fieldNames();

private:
  // unsigned long grid_id;

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
