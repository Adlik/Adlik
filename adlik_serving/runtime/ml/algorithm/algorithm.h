// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_H

#include <functional>

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

  virtual cub::StatusWrapper run(const ::google::protobuf::Any&,
                                 ::google::protobuf::Any&,
                                 std::function<bool(void)> should_terminate) = 0;
};

#define TERMINATE_IF()                                                                      \
  {                                                                                         \
    if (should_terminate()) {                                                               \
      return cub::StatusWrapper(cub::Internal, "Forced to terminate task, maybe timeout!"); \
    }                                                                                       \
  }

constexpr char DEFAULT_MODEL[] = "model.pbtxt";

}  // namespace ml_runtime

#endif
