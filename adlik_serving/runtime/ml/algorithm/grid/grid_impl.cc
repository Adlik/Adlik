// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <unordered_map>
#include <vector>

#include "adlik_serving/framework/manager/time_stats.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_register.h"
#include "adlik_serving/runtime/ml/algorithm/grid/grid_input.h"
#include "adlik_serving/runtime/ml/algorithm/grid/grid_output.h"
#include "adlik_serving/runtime/ml/algorithm/grid/kmeans.h"
#include "adlik_serving/runtime/ml/algorithm/proto/task_config.pb.h"
#include "cub/log/log.h"
#include "cub/string/str_utils.h"
#include "dlib/clustering.h"
#include "dlib/rand.h"

namespace adlik {
namespace serving {
struct AlgorithmConfig;

}  // namespace serving
}  // namespace adlik

namespace ml_runtime {

using namespace adlik::serving;

struct NeighborClassKey {
  NeighborClassKey(const GridInput& i) {
    std::vector<std::string> str1 = {
        std::to_string(i.neighPLMN_intra1), std::to_string(i.neighNB_intra1), std::to_string(i.neighCID_intra1)};
    std::vector<std::string> str2 = {
        std::to_string(i.neighPLMN_intra2), std::to_string(i.neighNB_intra2), std::to_string(i.neighCID_intra2)};
    std::vector<std::string> str = {cub::strutils::join(str1, "-"), cub::strutils::join(str2, "-")};
    key = cub::strutils::join(str, "&");
  }

  std::string key;
};

bool operator==(const NeighborClassKey& lhs, const NeighborClassKey& rhs) {
  return lhs.key == rhs.key;
}

bool operator!=(const NeighborClassKey& lhs, const NeighborClassKey& rhs) {
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const NeighborClassKey& obj) {
  os << obj.key;
  return os;
}

using NeighborClassValue = std::vector<size_t>;  // indexes of samples of the specific class

struct RsrpClassKey {
  RsrpClassKey(const GridInput& i)
      : rsrp1(lambda(i.serving_rsrp)), rsrp2(lambda(i.neighRSRP_intra1)), rsrp3(lambda(i.neighRSRP_intra2)) {
    std::vector<std::string> str = {std::to_string(rsrp1), std::to_string(rsrp2), std::to_string(rsrp3)};

    key = cub::strutils::join(str, ",");
  }
  Rsrp rsrp1;
  Rsrp rsrp2;
  Rsrp rsrp3;
  std::string key;

private:
  const Rsrp GRID_SIZE = 3;
#define POSITIVE_RSRP(rsrp) (rsrp + 140)

  Rsrp lambda(Rsrp rsrp) {
    return (Rsrp)((POSITIVE_RSRP(rsrp)) / GRID_SIZE);
  }
};

bool operator==(const RsrpClassKey& lhs, const RsrpClassKey& rhs) {
  return lhs.rsrp1 == rhs.rsrp1 && lhs.rsrp2 == rhs.rsrp2 && lhs.rsrp3 == rhs.rsrp3;
}

bool operator!=(const RsrpClassKey& lhs, const RsrpClassKey& rhs) {
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const RsrpClassKey& obj) {
  os << obj.key;
  return os;
}

}  // namespace ml_runtime

//////////////////////////////////////////////////////////////////////////////////////////////

namespace std {
template <>
struct hash<ml_runtime::NeighborClassKey> {
  std::size_t operator()(ml_runtime::NeighborClassKey const& s) const noexcept {
    return std::hash<std::string>{}(s.key);
  }
};

template <>
struct hash<ml_runtime::RsrpClassKey> {
  std::size_t operator()(ml_runtime::RsrpClassKey const& s) const noexcept {
    return std::hash<std::string>{}(s.key);
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

size_t estimateK(const GridInputs& ori_inputs, const NeighborClassValue& indexes) {
  std::unordered_map<RsrpClassKey, size_t> classes;
  for (const auto& i : indexes) {
    RsrpClassKey key(ori_inputs[i]);
    classes[key]++;
  }
  size_t k = 0;
  for (const auto& pair : classes) {
    if (pair.second < MIN_SAMPLES_PER_GRID)
      continue;
    k++;
  }
  return k;
}

void subdivideClass(const GridInputs& ori_inputs, const std::pair<NeighborClassKey, NeighborClassValue>& clazz) {
  if (clazz.second.size() < MIN_SAMPLES_PER_NEIGHBOR_CLASS) {
    // If there is too few samples of this class, no need to do kmeans
    return;
  }

  auto k = estimateK(ori_inputs, clazz.second);
  if (k <= 0)
    return;

  // do kmeans
  DEBUG_LOG << "Begin to do kmeans for class: " << clazz.first << ", total size: " << clazz.second.size()
            << ", k: " << k;

  using SampleType = dlib::matrix<double, 3, 1>;
  std::vector<SampleType> samples;
  for (const auto& i : clazz.second) {
    SampleType s;
    s(0) = ori_inputs[i].serving_rsrp;
    s(1) = ori_inputs[i].neighRSRP_intra1;
    s(2) = ori_inputs[i].neighRSRP_intra2;
    // todo: should drop some marginal samples?
    samples.push_back(s);
  }
  Kmeans<SampleType> kmeans(k, KMEANS_MAX_ITER);
  auto y = kmeans.fit(samples);
  const auto& centers = kmeans.getCenters();

  // todo: there also need centers

  // statistics
}

}  // namespace

cub::StatusWrapper Grid::run(const adlik::serving::TaskReq& req, adlik::serving::TaskRsp&) {
  DEBUG_LOG << "Begin to run grid algorithm";

  GridInputs total_inputs;
  std::unordered_map<NeighborClassKey, NeighborClassValue> classes;
  auto func = [&](size_t index, const GridInput& i) {
    NeighborClassKey key(i);
    classes[key].push_back(index);
  };

  {
    TimeStats stats("Load input csv and do initial screen");
    auto status = loadGridInput(req.grid().input(), total_inputs, func);
    if (!status.ok() || total_inputs.size() == 0) {
      ERR_LOG << "Load Grid input failure: " << status.error_message();
      return status;
    }
  }

  DEBUG_LOG << "Grid input size: " << total_inputs.size() << ", initial screening: " << classes.size();

  for (const auto& c : classes) {
    subdivideClass(total_inputs, c);
    // break;
  }
  return cub::StatusWrapper::OK();
}

REGISTER_ALGORITHM(Grid, "grid");

}  // namespace ml_runtime
