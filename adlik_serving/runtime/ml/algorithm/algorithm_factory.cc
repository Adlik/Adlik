// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/algorithm_factory.h"

#include "cub/log/log.h"

namespace ml_runtime {

cub::StatusWrapper AlgorithmFactory::create(const std::string& name,
                                            const std::string& model_dir,
                                            std::unique_ptr<Algorithm>* algo) {
  auto it = creators.find(name);
  return it != creators.end() ?
             it->second(model_dir, algo) :
             cub::StatusWrapper(cub::Internal, "Not found algorithm creator by name \"" + name + "\"");
}

void AlgorithmFactory::add(const std::string& name, AlgoCreator creator) {
  creators.insert({name, creator});
}

}  // namespace ml_runtime
