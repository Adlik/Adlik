// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/grid/grid_output.h"

#include <algorithm>

#include "adlik_serving/runtime/ml/algorithm/grid/grid_input.h"
#include "adlik_serving/runtime/ml/algorithm/grid/neighbors.h"

namespace ml_runtime {

namespace {

const size_t MAX_NEIGHBOR_NUM = 10;

static std::vector<std::string> HALF_HEADER = {"ServerRSRP_Core",
                                               "ServerRSRP_MAX",
                                               "ServerRSRP_MIN",

                                               "NeighPLMN_intra1",
                                               "NeighgNB_intra1",
                                               "NeighCID_intra1",
                                               "NeighCellRSRP1_intra_Core",
                                               "NeighCellRSRP1_intra_MAX",
                                               "NeighCellRSRP1_intra_MIN",

                                               "NeighPLMN_intra2",
                                               "NeighgNB_intra2",
                                               "NeighCID_intra2",
                                               "NeighCellRSRP2_intra_Core",
                                               "NeighCellRSRP2_intra_MAX",
                                               "NeighCellRSRP2_intra_MIN",

                                               "NeighCellNum_intra"};

static std::vector<std::string> neighbor_header = {"NeighPLMN_intra_Sta",
                                                   "NeighgNB_intra_Sta",
                                                   "NeighCID_intra_Sta",
                                                   "Event1Num_Sta",
                                                   "Event2Num_Sta",
                                                   "Event3Num_Sta"};

}  // namespace

GridOutput::GridOutput()
    : serverRSRP_core(0),
      serverRSRP_max(MIN_RSRP),
      serverRSRP_min(MAX_RSRP),
      neighRSRP_core1(0),
      neighRSRP_max1(MIN_RSRP),
      neighRSRP_min1(MAX_RSRP),
      neighRSRP_core2(0),
      neighRSRP_max2(MIN_RSRP),
      neighRSRP_min2(MAX_RSRP),
      neighbor_num(0) {
}

GridOutput::GridOutput(const Neighbors& neighbors,
                       const Rsrp serverRSRP_core,
                       const Rsrp neighRSRP_core1,
                       const Rsrp neighRSRP_core2)
    : serverRSRP_core(serverRSRP_core),
      serverRSRP_max(MIN_RSRP),
      serverRSRP_min(MAX_RSRP),
      neighbor1(neighbors.neighbor1),
      neighRSRP_core1(neighRSRP_core1),
      neighRSRP_max1(MIN_RSRP),
      neighRSRP_min1(MAX_RSRP),
      neighbor2(neighbors.neighbor2),
      neighRSRP_core2(neighRSRP_core2),
      neighRSRP_max2(MIN_RSRP),
      neighRSRP_min2(MAX_RSRP),
      neighbor_num(0) {
}

void GridOutput::update(const GridInput& input) {
  serverRSRP_max = std::max(serverRSRP_max, input.serving_rsrp);
  serverRSRP_min = std::min(serverRSRP_min, input.serving_rsrp);
  neighRSRP_max1 = std::max(neighRSRP_max1, input.neighRSRP_intra1);
  neighRSRP_min1 = std::min(neighRSRP_min1, input.neighRSRP_intra1);
  neighRSRP_max2 = std::max(neighRSRP_max2, input.neighRSRP_intra2);
  neighRSRP_min2 = std::min(neighRSRP_min2, input.neighRSRP_intra2);

  auto it =
      std::find_if(stats.begin(), stats.end(), [&](const Statistics& s) { return s.neighbor == input.neighbor3; });
  Statistics* stat = nullptr;
  if (it != stats.end()) {
    stat = &(*it);
  } else {
    GridOutput::Statistics temp_stat;
    temp_stat.neighbor = input.neighbor3;
    stats.push_back(std::move(temp_stat));
    neighbor_num++;
    stat = &stats[stats.size() - 1];
  }

  if (input.event == 1) {
    stat->event_1++;
  } else if (input.event == 2) {
    stat->event_2++;
  } else if (input.event == 3) {
    stat->event_3++;
  } else {
  }
}

void GridOutput::arrangeStats() {
  neighbor_num = std::min(MAX_NEIGHBOR_NUM, stats.size());
  std::sort(stats.begin(), stats.end(), [](const GridOutput::Statistics& lhs, const GridOutput::Statistics& rhs) {
    return lhs.event_1 + lhs.event_2 + lhs.event_3 > rhs.event_1 + rhs.event_2 + rhs.event_3;
  });
}

std::vector<std::string> GridOutput::toString() const {
  std::vector<std::string> vec;
  auto f = [&](auto value) { return std::to_string(value); };

  vec.push_back(f(serverRSRP_core));
  vec.push_back(f(serverRSRP_max));
  vec.push_back(f(serverRSRP_min));

  vec.push_back(f(neighbor1.plmn));
  vec.push_back(f(neighbor1.nb));
  vec.push_back(f(neighbor1.cell));
  vec.push_back(f(neighRSRP_core1));
  vec.push_back(f(neighRSRP_max1));
  vec.push_back(f(neighRSRP_min1));

  vec.push_back(f(neighbor2.plmn));
  vec.push_back(f(neighbor2.nb));
  vec.push_back(f(neighbor2.cell));
  vec.push_back(f(neighRSRP_core2));
  vec.push_back(f(neighRSRP_max2));
  vec.push_back(f(neighRSRP_min2));

  vec.push_back(f(neighbor_num));
  for (size_t i = 0; i < neighbor_num && i < this->stats.size(); ++i) {
    vec.push_back(f(stats[i].neighbor.plmn));
    vec.push_back(f(stats[i].neighbor.nb));
    vec.push_back(f(stats[i].neighbor.cell));
    vec.push_back(f(stats[i].event_1));
    vec.push_back(f(stats[i].event_2));
    vec.push_back(f(stats[i].event_3));
  }

  return vec;
}

std::vector<std::string> GridOutput::fieldNames() {
  std::vector<std::string> header(HALF_HEADER);
  for (size_t i = 1; i < MAX_NEIGHBOR_NUM + 1; ++i) {
    for (const auto& h : neighbor_header) {  // todo: will be optimized by std::transform in c++17 & c++20
      header.push_back(h + std::to_string(i));
    }
  }
  return header;
}

}  // namespace ml_runtime
