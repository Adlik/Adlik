// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <unordered_map>
#include <vector>

#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_register.h"
#include "adlik_serving/runtime/ml/algorithm/grid/grid_input.h"
#include "adlik_serving/runtime/ml/algorithm/grid/grid_output.h"
#include "adlik_serving/runtime/ml/algorithm/grid/kmeans.h"
#include "adlik_serving/runtime/ml/algorithm/proto/task_config.pb.h"
#include "cub/log/log.h"
#include "dlib/clustering.h"
#include "dlib/rand.h"

namespace adlik {
namespace serving {
struct AlgorithmConfig;

}  // namespace serving
}  // namespace adlik

namespace ml_runtime {

struct NeighborClassKey {
  NeighborClassKey(const GridInput& i) : neighbor1(i.intra_neighbor1), neighbor2(i.intra_neighbor2) {
  }

  CellId neighbor1;
  CellId neighbor2;
};

bool operator==(const NeighborClassKey& lhs, const NeighborClassKey& rhs) {
  return lhs.neighbor1 == rhs.neighbor1 && lhs.neighbor2 == rhs.neighbor2;
}

bool operator!=(const NeighborClassKey& lhs, const NeighborClassKey& rhs) {
  return !(lhs == rhs);
}

using NeighborClassValue = std::vector<size_t>;

struct RsrpClassKey {
  RsrpClassKey(const GridInput& i)
      : rsrp1(lambda(i.serving_rsrp)), rsrp2(lambda(i.intra_rsrp1)), rsrp3(lambda(i.intra_rsrp2)) {
  }
  Rsrp rsrp1;
  Rsrp rsrp2;
  Rsrp rsrp3;

private:
  const Rsrp GRID_SIZE = 3;
#define POSITIVE_RSRP(rsrp) (rsrp + 140)

  Rsrp lambda(Rsrp rsrp) {
    return Rsrp(POSITIVE_RSRP(rsrp) / GRID_SIZE);
  }
};

bool operator==(const RsrpClassKey& lhs, const RsrpClassKey& rhs) {
  return lhs.rsrp1 == rhs.rsrp1 && lhs.rsrp2 == rhs.rsrp2 && lhs.rsrp3 == rhs.rsrp3;
}

bool operator!=(const RsrpClassKey& lhs, const RsrpClassKey& rhs) {
  return !(lhs == rhs);
}

}  // namespace ml_runtime

//////////////////////////////////////////////////////////////////////////////////////////////

namespace std {
template <>
struct hash<ml_runtime::NeighborClassKey> {
  std::size_t operator()(ml_runtime::NeighborClassKey const& s) const noexcept {
    return ((unsigned int)s.neighbor1) << 16 | (unsigned int)s.neighbor2;
  }
};

template <>
struct hash<ml_runtime::RsrpClassKey> {
  std::size_t operator()(ml_runtime::RsrpClassKey const& s) const noexcept {
    return ((std::size_t)s.rsrp1) << 32 | ((std::size_t)s.rsrp2) << 16 | (std::size_t)s.rsrp3;
  }
};

}  // namespace std

//////////////////////////////////////////////////////////////////////////////////////////////

namespace ml_runtime {

struct Grid : Algorithm {
  static void create(const adlik::serving::AlgorithmConfig&, std::unique_ptr<Algorithm>* algorithm) {
    *algorithm = std::make_unique<Grid>();
  }

  cub::StatusWrapper run(const adlik::serving::TaskReq&, adlik::serving::TaskRsp&) override;

  const std::string name() const override {
    return "grid";
  }
};

namespace {

const size_t MIN_SAMPLES_PER_NEIGHBOR_CLASS = 100;
const size_t MIN_SAMPLES_PER_GRID = 25;

const long KMEANS_MAX_ITER = 100;

std::unordered_map<NeighborClassKey, NeighborClassValue> screen(const std::vector<GridInput>& inputs) {
  std::unordered_map<NeighborClassKey, NeighborClassValue> classes;
  for (size_t i = 0; i < inputs.size(); ++i) {
    NeighborClassKey key(inputs[i]);
    classes[key].push_back(i);
  }
  return classes;
}

size_t estimateK(const GridInputs& inputs) {
  std::unordered_map<RsrpClassKey, size_t> classes;
  for (const auto& it : inputs) {
    RsrpClassKey key(it);
    classes[key]++;
  }
  size_t k = 0;
  for (const auto& it : classes) {
    if (it.second < MIN_SAMPLES_PER_GRID)
      continue;
    k++;
  }
  return k;
}

void doOneClass(const GridInputs& inputs) {
  if (inputs.size() < MIN_SAMPLES_PER_NEIGHBOR_CLASS)
    return;

  // estimate k
  auto k = estimateK(inputs);
  DEBUG_LOG << "K = " << k;
  if (k <= 0)
    return;

  // do kmeans
  using SampleType = dlib::matrix<double, 3, 1>;
  std::vector<SampleType> samples;
  for (const auto& i : inputs) {
    SampleType s;
    s(0) = i.serving_rsrp;
    s(1) = i.intra_rsrp1;
    s(2) = i.intra_rsrp2;
    // todo: should drop some marginal samples?
    samples.push_back(s);
  }
  Kmeans<SampleType> kmeans;
  auto y = kmeans(k, samples, KMEANS_MAX_ITER);
  // todo: there also need centers

  // statistics
}

}  // namespace

cub::StatusWrapper Grid::run(const adlik::serving::TaskReq& req, adlik::serving::TaskRsp&) {
  GridInputs inputs;
  auto status = loadGridInput(req.grid().input(), inputs);
  if (!status.ok() || inputs.size() == 0) {
    ERR_LOG << "Load Grid input failure: " << status.error_message();
    return status;
  }

  DEBUG_LOG << "Grid input size: " << inputs.size();

  auto neighbor_classes = screen(inputs);
  for (const auto& c : neighbor_classes) {
    std::vector<GridInput> input;
    for (auto& index : c.second) {
      input.push_back(inputs[index]);
      doOneClass(input);
    }
  }

  return cub::StatusWrapper::OK();
}

REGISTER_ALGORITHM(Grid, "grid");

}  // namespace ml_runtime
