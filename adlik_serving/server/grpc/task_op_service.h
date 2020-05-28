// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_SERVER_GRPC_TASK_OP_SERVICE_H
#define ADLIK_SERVING_SERVER_GRPC_TASK_OP_SERVICE_H

#include "adlik_serving/apis/task_service.grpc.pb.h"
#include "cub/base/status.h"
#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct TaskOpImpl;
struct CreateTaskRequest;
struct CreateTaskResponse;

struct TaskOpService : TaskService::Service {
private:
  ::grpc::Status create(::grpc::ServerContext*, const CreateTaskRequest*, CreateTaskResponse*) override;
  USE_ROLE(TaskOpImpl);
};

}  // namespace serving
}  // namespace adlik

#endif
