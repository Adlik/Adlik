// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIMEK_ML_ALGORITHM_ALGORITHM_REGISTER_H
#define ADLIK_SERVING_RUNTIMEK_ML_ALGORITHM_ALGORITHM_REGISTER_H

#include "adlik_serving/runtime/ml/algorithm/algorithm_creator.h"

#include <string>

namespace ml_runtime {

struct AlgorithmRegister {
  AlgorithmRegister(const std::string& name, AlgoCreator);
};

#define REGISTER_ALGORITHM(AlgorithmClass, name)                                                                      \
  static AlgorithmRegister register__##cnt(name,                                                                      \
                                           [](const AlgorithmConfig& config, std::unique_ptr<Algorithm>* algorithm) { \
                                             AlgorithmClass::create(config, algorithm);                               \
                                           })

}  // namespace ml_runtime

#endif
