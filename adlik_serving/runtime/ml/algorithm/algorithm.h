// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_H

#include "cub/base/status_wrapper.h"

namespace google {
namespace protobuf {
struct Any;
}
}  // namespace google

namespace ml_runtime {

struct Algorithm {
  virtual ~Algorithm() {
  }

  virtual cub::StatusWrapper run(const ::google::protobuf::Any&, ::google::protobuf::Any&) = 0;
};

constexpr char DEFAULT_MODEL[] = "model.pbtxt";

}  // namespace ml_runtime

#endif
