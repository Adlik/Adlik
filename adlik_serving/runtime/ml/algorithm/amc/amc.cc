// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <memory>

#include "adlik_serving/framework/domain/algorithm_config.pb.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_register.h"
#include "adlik_serving/runtime/ml/algorithm/proto/task_config.pb.h"
#include "cub/log/log.h"

namespace ml_runtime {

struct Amc : Algorithm {
  static void create(const adlik::serving::AlgorithmConfig&, std::unique_ptr<Algorithm>*);

  Amc(const adlik::serving::AmcConfig& config)
      : config(config), target_prime((1 - config.nbler_target()) / config.nbler_target()) {
  }

  cub::StatusWrapper run(const adlik::serving::TaskReq&, adlik::serving::TaskRsp&) override;

  const std::string name() const override {
    return "amc";
  }

private:
  adlik::serving::AmcConfig config;
  double target_prime;
};

void Amc::create(const adlik::serving::AlgorithmConfig& config, std::unique_ptr<Algorithm>* algorithm) {
  if (config.has_amc_config()) {
    if (config.amc_config().nbler_target() == 0) {
      ERR_LOG << "nbler_target is 0!";
      return;
    }
    *algorithm = std::make_unique<Amc>(config.amc_config());
  }
  return;
}

cub::StatusWrapper Amc::run(const adlik::serving::TaskReq& req, adlik::serving::TaskRsp& rsp) {
  DEBUG_LOG << "Prepare to run amc";

  auto result = rsp.mutable_amc();
  auto deltas = result->mutable_deltas();

  for (const auto& it : req.amc().blers()) {
    deltas->insert({it.first, (1 - it.second - it.second * target_prime) * config.lambda()});
  }

  return cub::StatusWrapper::OK();
}

REGISTER_ALGORITHM(Amc, "amc");

}  // namespace ml_runtime
