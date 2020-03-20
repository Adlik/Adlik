// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_CREATOR_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_CREATOR_H

#include <functional>
#include <memory>
#include <string>

#include "cub/base/status_wrapper.h"

namespace ml_runtime {

struct Algorithm;

using AlgoCreator = std::function<cub::StatusWrapper(const std::string&, std::unique_ptr<Algorithm>*)>;

}  // namespace ml_runtime

#endif
