// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/manager/runtime.h"

#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

tensorflow::Status Runtime::predict(const RunOptions&, ModelHandle*, const PredictRequest&, PredictResponse&) {
  return tensorflow::errors::Internal("predict function not implemented!");
}

cub::StatusWrapper Runtime::createTask(const RunOptions&, ModelHandle*, const CreateTaskRequest&, CreateTaskResponse&) {
  return cub::StatusWrapper(cub::Internal, "createTask function not implemented!");
}

}  // namespace serving
}  // namespace adlik
