// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/grpc/grpc_server.h"

#include <map>

#include "cub/base/assertions.h"
#include "grpcpp/server.h"
#include "grpcpp/server_context.h"
#include "grpcpp/support/status.h"

namespace adlik {
namespace serving {

cub::Status GrpcServer::start() {
  if (build(builder)) {
    builder.RegisterService(static_cast<GrpcService*>(this));
    builder.RegisterService(static_cast<TaskOpService*>(this));
    server = builder.BuildAndStart();
    CUB_ASSERT_VALID_PTR(server);
    INFO_LOG << "grpc server is serving...";
  } else {
    INFO_LOG << "grpc server not started";
  }
  return cub::Success;
}

void GrpcServer::wait() {
  if (server != nullptr) {
    server->Wait();
  }
}

}  // namespace serving
}  // namespace adlik
