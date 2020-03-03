// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_CREATOR_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_CREATOR_H

#include <functional>
#include <memory>

namespace ml_runtime {

struct Algorithm;
struct AlgorithmConfig;

using AlgoCreator = std::function<void(const AlgorithmConfig&, std::unique_ptr<Algorithm>*)>;

}  // namespace ml_runtime

#endif
