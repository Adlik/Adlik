// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/algorithm/algorithm.h"
#include "cub/log/log.h"

namespace ml_runtime {

struct Amc : Algorithm {
  static void create(const adlik::serving::AlgorithmConfig&, std::unique_ptr<Algorithm>*);

  void run(MLTask&) override;

  const std::string name() const override {
    return "amc";
  }
};

void Amc::create(const adlik::serving::AlgorithmConfig&, std::unique_ptr<Algorithm>* algorithm) {
}

void Amc::run(MLTask& task) override {
  DEBUG_LOG << "Prepare to run amc";
}

}  // namespace ml_runtime
