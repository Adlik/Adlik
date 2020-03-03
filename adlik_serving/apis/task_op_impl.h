// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_APIS_TASK_OP_IMPL_H
#define ADLIK_SERVING_APIS_TASK_OP_IMPL_H

#include <string>

#include "cub/base/status_wrapper.h"
#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct ServingStore;
struct ModelStore;
struct ManagedStore;
struct CreateTaskRequest;
struct CreateTaskResponse;
struct RunOptions;

struct TaskOpImpl {
  virtual ~TaskOpImpl() = default;

  cub::StatusWrapper create(const RunOptions&, const CreateTaskRequest&, CreateTaskResponse&);

private:
  USE_ROLE(ServingStore);
  USE_ROLE(ModelStore);
  USE_ROLE(ManagedStore);
};
}  // namespace serving
}  // namespace adlik

#endif
