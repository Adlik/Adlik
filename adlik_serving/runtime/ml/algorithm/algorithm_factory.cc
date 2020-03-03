// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/algorithm_factory.h"

namespace ml_runtime {

void AlgorithmFactory::create(const std::string& name,
                              const AlgorithmConfig& config,
                              std::unique_ptr<Algorithm>* algo) {
  auto it = creators.find(name);
  if (it != creators.end())
    it->second(config, algo);
}

void AlgorithmFactory::add(const std::string& name, AlgoCreator creator) {
  creators.insert({name, creator});
}

}  // namespace ml_runtime
