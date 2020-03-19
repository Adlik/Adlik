// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ALGORITHM_H

#include <string>

#include "cub/base/status_wrapper.h"

namespace adlik {
namespace serving {
struct TaskReq;
struct TaskRsp;
}  // namespace serving
}  // namespace adlik

namespace google {
namespace protobuf {
struct Any;
}
}  // namespace google

namespace ml_runtime {

struct MLTask;

struct Algorithm {
  virtual ~Algorithm() {
  }

  virtual const std::string name() const = 0;
  virtual cub::StatusWrapper run(const ::google::protobuf::Any&, ::google::protobuf::Any&) = 0;
};

}  // namespace ml_runtime

#endif
