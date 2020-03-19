// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <unordered_map>
#include <vector>

#include "adlik_serving/apis/grid_task.pb.h"
#include "adlik_serving/framework/domain/algorithm_config.pb.h"
#include "adlik_serving/framework/manager/time_stats.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_register.h"
#include "adlik_serving/runtime/ml/algorithm/grid/grid_csv_saver.h"
#include "adlik_serving/runtime/ml/algorithm/grid/grid_input.h"
#include "adlik_serving/runtime/ml/algorithm/grid/grid_output.h"
#include "adlik_serving/runtime/ml/algorithm/grid/kmeans.h"
#include "adlik_serving/runtime/ml/algorithm/grid/neighbors.h"
#include "adlik_serving/runtime/ml/algorithm/grid/rsrp_grid.h"
#include "cub/env/fs/file_system.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"
#include "google/protobuf/any.pb.h"

namespace ml_runtime {

struct GridAlgorithm : Algorithm {
  static void create(const adlik::serving::AlgorithmConfig& config, std::unique_ptr<Algorithm>* algorithm);

  GridAlgorithm(const adlik::serving::GridConfig& config) : config(config) {
  }

  cub::StatusWrapper run(const ::google::protobuf::Any&, ::google::protobuf::Any&) override;

  const std::string name() const override {
    return "grid";
  }

private:
  using InputPtrs = std::vector<const GridInput*>;  // samples of a specific class
  using SampleType = dlib::matrix<double, 3, 1>;

  std::unordered_map<Neighbors, InputPtrs> initialScreen(const GridInputs& total);
  void subdivideClass(const Neighbors& neighbors, const InputPtrs& inputs, GridCsvSaver& saver);

  std::vector<SampleType> makeKmeansInput(const InputPtrs& inputs);
  size_t estimateK(const InputPtrs& inputs);
  std::vector<GridOutput> makeOutput(const Neighbors& neighbors,
                                     const InputPtrs& inputs,
                                     const std::vector<unsigned long>& lables,
                                     const std::vector<SampleType>& centers);

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

std::vector<GridAlgorithm::SampleType> GridAlgorithm::makeKmeansInput(const InputPtrs& inputs) {
  std::vector<SampleType> samples;
  for (const auto& i : inputs) {
    SampleType s;
    s(0) = i->serving_rsrp;
    s(1) = i->neighRSRP_intra1;
    s(2) = i->neighRSRP_intra2;
    // todo: should drop some marginal samples?
    samples.push_back(s);
  }
  return samples;
}

std::vector<GridOutput> GridAlgorithm::makeOutput(const Neighbors& neighbors,
                                                  const GridAlgorithm::InputPtrs& inputs,
                                                  const std::vector<unsigned long>& lables,
                                                  const std::vector<GridAlgorithm::SampleType>& centers) {
  std::vector<GridOutput> outputs(centers.size());
  for (size_t i = 0; i < centers.size(); ++i) {
    // outputs[i].grid_id = i;
    outputs[i] = GridOutput(neighbors, (Rsrp)centers[i](0), (Rsrp)centers[i](1), (Rsrp)centers[i](2));
  }

  for (size_t i = 0; i < inputs.size(); ++i) {
    const auto& s = *inputs[i];
    auto lable = lables[i];
    auto& output = outputs[lable];
    output.update(s);
  }

  // Need ??
  for (size_t i = 0; i < centers.size(); ++i) {
    outputs[i].arrangeStats();
  }
  return outputs;
}

void GridAlgorithm::subdivideClass(const Neighbors& neighbors, const InputPtrs& inputs, GridCsvSaver& saver) {
  if (inputs.size() < config.min_samples_per_neighbor_group()) {
    // If there is too few samples of this class, no need to do kmeans
    return;
  }

  auto k = estimateK(inputs);
  if (k <= 0)
    return;

  // do kmeans
  DEBUG_LOG << "Begin to do kmeans for class: " << neighbors << ", samples size: " << inputs.size() << ", k: " << k;

  auto samples = makeKmeansInput(inputs);
  Kmeans<SampleType> kmeans(k, config.kmeans_max_iter());
  auto lables = kmeans.fit(samples);
  const auto& centers = kmeans.getCenters();

  auto outputs = makeOutput(neighbors, inputs, lables, centers);
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

cub::StatusWrapper GridAlgorithm::run(const ::google::protobuf::Any& req_detail, ::google::protobuf::Any& rsp_detail) {
  if (!req_detail.Is<::adlik::serving::GridTaskReq>()) {
    return cub::StatusWrapper(cub::InvalidArgument, "Input doesn't contain grid task config!");
  }

  adlik::serving::GridTaskReq grid;
  req_detail.UnpackTo(&grid);

  if (!cub::filesystem().exists(grid.input())) {
    return cub::StatusWrapper(cub::InvalidArgument, "Input file doesn't exist");
  }
  auto input_path = cub::Path(grid.input());
  if (input_path.extName() != "csv") {
    return cub::StatusWrapper(cub::InvalidArgument, "Input file isn't csv");
  }

  auto output_path = cub::Path(grid.output());
  auto dir = output_path.dirName();
  if (!cub::filesystem().exists(dir.to_s())) {
    return cub::StatusWrapper(cub::InvalidArgument, "Output directory doesn't exist");
  }

  GridInputs total_inputs;
  {
    adlik::serving::TimeStats stats("Load input csv");
    auto status = loadGridInput(grid.input(), total_inputs);
    if (!status.ok() || total_inputs.size() == 0) {
      ERR_LOG << "Load Grid input failure: " << status.error_message();
      return status;
    }
  }

  auto classes = initialScreen(total_inputs);
  DEBUG_LOG << "Grid input size: " << total_inputs.size() << ", initial screening: " << classes.size();

  GridCsvSaver saver(grid.output());
  for (const auto& c : classes) {
    subdivideClass(c.first, c.second, saver);
  }

  adlik::serving::GridTaskRsp grid_output;
  grid_output.set_output(grid.output());
  rsp_detail.PackFrom(grid_output);
  return cub::StatusWrapper::OK();
}

REGISTER_ALGORITHM(GridAlgorithm, "grid");

}  // namespace ml_runtime
