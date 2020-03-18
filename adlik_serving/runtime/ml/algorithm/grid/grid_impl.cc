// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <unordered_map>
#include <vector>

#include "adlik_serving/framework/domain/algorithm_config.pb.h"
#include "adlik_serving/framework/manager/time_stats.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_register.h"
#include "adlik_serving/runtime/ml/algorithm/grid/grid_input.h"
#include "adlik_serving/runtime/ml/algorithm/grid/grid_output.h"
#include "adlik_serving/runtime/ml/algorithm/grid/kmeans.h"
#include "adlik_serving/runtime/ml/algorithm/grid/neighbors.h"
#include "adlik_serving/runtime/ml/algorithm/grid/rsrp_grid.h"
#include "adlik_serving/runtime/ml/algorithm/proto/task_config.pb.h"
#include "cub/log/log.h"
#include "cub/string/str_utils.h"
#include "dlib/clustering.h"
#include "dlib/rand.h"

namespace ml_runtime {

struct GridAlgorithm : Algorithm {
  static void create(const adlik::serving::AlgorithmConfig& config, std::unique_ptr<Algorithm>* algorithm);

  GridAlgorithm(const adlik::serving::GridConfig& config) : config(config) {
  }

  cub::StatusWrapper run(const adlik::serving::TaskReq&, adlik::serving::TaskRsp&) override;

  const std::string name() const override {
    return "grid";
  }

private:
  using InputPtrs = std::vector<const GridInput*>;  // samples of a specific class

  size_t estimateK(const InputPtrs& inputs);
  void subdivideClass(const std::pair<Neighbors, InputPtrs>& clazz, GridCsvSaver& saver);
  std::unordered_map<Neighbors, InputPtrs> initialScreen(const GridInputs& total);

  adlik::serving::GridConfig config;
};

void GridAlgorithm::create(const adlik::serving::AlgorithmConfig& config, std::unique_ptr<Algorithm>* algorithm) {
  if (config.has_grid_config()) {
    const auto& grid = config.grid_config();
    if (grid.min_samples_per_neighbor_group() == 0 || grid.min_samples_per_grid() == 0 || grid.init_grid_size() == 0 ||
        grid.kmeans_max_iter() == 0) {
      ERR_LOG << "input config invalid!";
      return;
    }
    *algorithm = std::make_unique<GridAlgorithm>(grid);
  }
}

size_t GridAlgorithm::estimateK(const InputPtrs& inputs) {
  std::unordered_map<RsrpGrid, size_t> classes;
  for (const auto& i : inputs) {
    RsrpGrid key((Rsrp)config.init_grid_size(), *i);
    classes[key]++;
  }
  size_t k = 0;
  for (const auto& pair : classes) {
    if (pair.second < config.min_samples_per_grid())
      continue;
    k++;
  }
  return k;
}

void GridAlgorithm::subdivideClass(const std::pair<Neighbors, InputPtrs>& clazz, GridCsvSaver& saver) {
  if (clazz.second.size() < config.min_samples_per_neighbor_group()) {
    // If there is too few samples of this class, no need to do kmeans
    return;
  }

  auto k = estimateK(clazz.second);
  if (k <= 0)
    return;

  // do kmeans
  DEBUG_LOG << "Begin to do kmeans for class: " << clazz.first << ", samples size: " << clazz.second.size()
            << ", k: " << k;

  using SampleType = dlib::matrix<double, 3, 1>;
  std::vector<SampleType> samples;
  for (const auto& i : clazz.second) {
    SampleType s;
    s(0) = i->serving_rsrp;
    s(1) = i->neighRSRP_intra1;
    s(2) = i->neighRSRP_intra2;
    // todo: should drop some marginal samples?
    samples.push_back(s);
  }
  Kmeans<SampleType> kmeans(k, config.kmeans_max_iter());
  auto y = kmeans.fit(samples);
  const auto& centers = kmeans.getCenters();

  std::vector<GridResult> outputs(k);
  for (unsigned long i = 0; i < k; ++i) {
    // outputs[i].grid_id = i;
    outputs[i].neighbor1 = clazz.first.neighbor1;
    outputs[i].neighbor2 = clazz.first.neighbor2;
    outputs[i].serverRSRP_core = (Rsrp)centers[i](0);
    outputs[i].neighRSRP_core1 = (Rsrp)centers[i](1);
    outputs[i].neighRSRP_core2 = (Rsrp)centers[i](2);
  }

  const auto& inputs = clazz.second;
  for (size_t i = 0; i < samples.size(); ++i) {
    const auto& s = *inputs[i];
    auto lable = y[i];
    auto& output = outputs[lable];
    output.serverRSRP_max = std::max(output.serverRSRP_max, s.serving_rsrp);
    output.serverRSRP_min = std::min(output.serverRSRP_min, s.serving_rsrp);
    output.neighRSRP_max1 = std::max(output.neighRSRP_max1, s.neighRSRP_intra1);
    output.neighRSRP_min1 = std::min(output.neighRSRP_min1, s.neighRSRP_intra1);
    output.neighRSRP_max2 = std::max(output.neighRSRP_max2, s.neighRSRP_intra2);
    output.neighRSRP_min2 = std::min(output.neighRSRP_min2, s.neighRSRP_intra2);

    bool found = false;
    for (auto& stat : output.stats) {
      if (stat.neighbor == s.neighbor3) {
        if (s.event == 1) {
          stat.event_1++;
        } else if (s.event == 2) {
          stat.event_2++;
        } else if (s.event == 3) {
          stat.event_3++;
        } else {
        }
        found = true;
        break;
      }
    }
    if (!found) {
      GridResult::Statistics stats;
      stats.neighbor = s.neighbor3;
      if (s.event == 1) {
        stats.event_1++;
      } else if (s.event == 2) {
        stats.event_2++;
      } else if (s.event == 3) {
        stats.event_3++;
      } else {
      }
      output.stats.push_back(std::move(stats));
    }
  }

  size_t max_neighbor = GridResult::maxNeighborNum();
  for (unsigned long i = 0; i < k; ++i) {
    outputs[i].neighbor_num = std::min(max_neighbor, outputs[i].stats.size());
    std::sort(outputs[i].stats.begin(),
              outputs[i].stats.end(),
              [](const GridResult::Statistics& lhs, const GridResult::Statistics& rhs) {
                return lhs.event_1 + lhs.event_2 + lhs.event_3 > rhs.event_1 + rhs.event_2 + rhs.event_3;
              });
  }

  saver.save(outputs);
}

std::unordered_map<Neighbors, GridAlgorithm::InputPtrs> GridAlgorithm::initialScreen(const GridInputs& total) {
  std::unordered_map<Neighbors, InputPtrs> classes;
  for (const auto& i : total) {
    Neighbors key(i);
    classes[key].push_back(&i);
  }
  return classes;
}

cub::StatusWrapper GridAlgorithm::run(const adlik::serving::TaskReq& req, adlik::serving::TaskRsp&) {
  GridInputs total_inputs;
  {
    adlik::serving::TimeStats stats("Load input csv and do initial screen");
    auto status = loadGridInput(req.grid().input(), total_inputs);
    if (!status.ok() || total_inputs.size() == 0) {
      ERR_LOG << "Load Grid input failure: " << status.error_message();
      return status;
    }
  }

  auto classes = initialScreen(total_inputs);
  DEBUG_LOG << "Grid input size: " << total_inputs.size() << ", initial screening: " << classes.size();

  std::unique_ptr<GridCsvSaver> saver;
  auto status = GridCsvSaver::create(req.grid().output(), &saver);
  if (!status.ok()) {
    return status;
  }

  for (const auto& c : classes) {
    subdivideClass(c, *saver);
  }
  return cub::StatusWrapper::OK();
}

REGISTER_ALGORITHM(GridAlgorithm, "grid");

}  // namespace ml_runtime
