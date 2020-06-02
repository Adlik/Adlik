// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H9AA9B39D_1EA4_4FD4_9DA2_CA91ACC24A9D
#define H9AA9B39D_1EA4_4FD4_9DA2_CA91ACC24A9D

#include <memory>

#include "adlik_serving/server/grpc/grpc_options.h"
#include "adlik_serving/server/grpc/grpc_service.h"
#include "adlik_serving/server/grpc/task_op_service.h"
#include "cub/base/status.h"
#include "grpcpp/server.h"
#include "grpcpp/server_builder.h"

namespace adlik {
namespace serving {

struct GrpcServer : GrpcOptions, GrpcService, TaskOpService {
  cub::Status start();
  void wait();

private:
  ::grpc::ServerBuilder builder;
  std::unique_ptr<grpc::Server> server;
};

}  // namespace serving
}  // namespace adlik

#endif
