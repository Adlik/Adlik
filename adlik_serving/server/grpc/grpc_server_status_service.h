// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_GPRC_SERVER_STATUS_SERVICE_H
#define ADLIK_SERVING_GRPC_SERVER_STATUS_SERVICE_H

#include "adlik_serving/apis/server_status_service.grpc.pb.h"

namespace adlik {
namespace serving {

struct GrpcServerStatusService : ServerStatusService::Service {
private:
  ::grpc::Status serverStatus(::grpc::ServerContext*, const ServerStatusRequest*, ServerStatusResponse*) override;
};

}  // namespace serving
}  // namespace adlik

#endif
