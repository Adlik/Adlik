// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/grpc/grpc_server_status_service.h"

#include "grpc/grpc.h"

namespace adlik {
namespace serving {

::grpc::Status GrpcServerStatusService::serverStatus(::grpc::ServerContext* ctxt,
                                                     const ServerStatusRequest* req,
                                                     ServerStatusResponse* rsp) {
  rsp->set_status("running");
  return ::grpc::Status::OK;
}

}  // namespace serving
}  // namespace adlik
