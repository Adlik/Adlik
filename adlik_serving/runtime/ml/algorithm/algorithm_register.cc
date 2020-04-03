// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/algorithm_register.h"

#include "adlik_serving/runtime/ml/algorithm/algorithm_factory.h"

namespace ml_runtime {

AlgorithmRegister::AlgorithmRegister(const std::string& name, AlgoCreator creator) {
  AlgorithmFactory::inst().add(name, creator);
}

}  // namespace ml_runtime
