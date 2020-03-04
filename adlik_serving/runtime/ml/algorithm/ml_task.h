// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ML_TASK_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ML_TASK_H

#include <string>

#include "cub/base/status_wrapper.h"

namespace ml_runtime {

struct KMeansTaskInternal {
  std::string input;
  size_t n_clusters;
  uint32_t max_iter;  // default 100
  bool compute_labels;  // default true
  std::string label_name;
  std::string output;
};

struct MLTask {
  cub::StatusWrapper status;
  KMeansTaskInternal kmeans;
};

}  // namespace ml_runtime

#endif
