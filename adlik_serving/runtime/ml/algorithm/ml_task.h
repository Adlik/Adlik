// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ML_TASK_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_ML_TASK_H

#include <string>

// #include "adlik_serving/apis/task.pb.h"
#include "cub/base/status_wrapper.h"

namespace adlik {
namespace serving {
struct CreateTaskRequest;
struct CreateTaskResponse;
}  // namespace serving
}  // namespace adlik

namespace ml_runtime {

struct MLTask {
  cub::StatusWrapper status;
  const adlik::serving::CreateTaskRequest* request;
  adlik::serving::CreateTaskResponse* response;

  size_t size() const {
    return 1;
  }
};

}  // namespace ml_runtime

#endif
