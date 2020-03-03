// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_H

#include <string>

namespace ml_runtime {

struct MLTask;

struct Algorithm {
  virtual ~Algorithm() {
  }

  virtual const std::string name() const = 0;
  virtual void run(MLTask&) = 0;
};

}  // namespace ml_runtime

#endif
