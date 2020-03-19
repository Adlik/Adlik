// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <memory>

#include "adlik_serving/apis/amc_task.pb.h"
#include "adlik_serving/framework/domain/algorithm_config.pb.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_register.h"
#include "cub/log/log.h"
#include "google/protobuf/any.pb.h"

namespace ml_runtime {

struct Amc : Algorithm {
  static void create(const adlik::serving::AlgorithmConfig&, std::unique_ptr<Algorithm>*);

  Amc(const adlik::serving::AmcConfig& config) : config(config) {
    init();
  }

  cub::StatusWrapper run(const ::google::protobuf::Any&, ::google::protobuf::Any&) override;

  const std::string name() const override {
    return "amc";
  }

private:
  struct Parameter {
    double target_prime;
    double lambda;
  };
  using CellId = unsigned int;

  void init() {
    for (const auto& it : config.cell_parameters()) {
      cells.insert({it.first, {(1 - it.second.nbler_target()) / it.second.nbler_target(), it.second.lambda()}});
    }
  }

  adlik::serving::AmcConfig config;

  std::unordered_map<CellId, Parameter> cells;
};

void Amc::create(const adlik::serving::AlgorithmConfig& config, std::unique_ptr<Algorithm>* algorithm) {
  if (config.has_amc_config()) {
    if (config.amc_config().cell_parameters().size() == 0) {
      ERR_LOG << "nbler_target is 0!";
      return;
    }
    *algorithm = std::make_unique<Amc>(config.amc_config());
  }
  return;
}

cub::StatusWrapper Amc::run(const ::google::protobuf::Any& request, ::google::protobuf::Any& rsp_detail) {
  DEBUG_LOG << "Prepare to run amc, this: " << this;

  if (!request.Is<::adlik::serving::AmcTaskReq>()) {
    return cub::StatusWrapper(cub::InvalidArgument, "Input doesn't contain amc task config!");
  }

  adlik::serving::AmcTaskReq task;
  request.UnpackTo(&task);

  adlik::serving::AmcTaskRsp output;
  output.set_cell_id(task.cell_id());

  auto search = cells.find(task.cell_id());
  if (search != cells.end()) {
    auto deltas = output.mutable_deltas();
    for (const auto& it : task.blers()) {
      auto delta = it.second;
      deltas->insert({it.first, (1 - delta - delta * search->second.target_prime) * search->second.lambda});
    }
    rsp_detail.PackFrom(output);
    return cub::StatusWrapper::OK();
  } else {
    ERR_LOG << "Not found input cell id: " << task.cell_id();
    return cub::StatusWrapper(cub::InvalidArgument, "Not found input cell id:");
  }
}

REGISTER_ALGORITHM(Amc, "amc");

}  // namespace ml_runtime
