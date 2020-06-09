// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_SERVER_GRPC_MODEL_OPERATE_SERVICE_H
#define ADLIK_SERVING_SERVER_GRPC_MODEL_OPERATE_SERVICE_H

#include "adlik_serving/apis/model_operate_service.grpc.pb.h"
#include "cub/base/status.h"
#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct ModelOperateImpl;

struct GrpcModelOperateService : ModelOperateService::Service {
private:
  ::grpc::Status addModel(::grpc::ServerContext*, const ModelOperateRequest*, ModelOperateResponse*) override;
  ::grpc::Status addModelVersion(::grpc::ServerContext*, const ModelOperateRequest*, ModelOperateResponse*) override;
  ::grpc::Status deleteModel(::grpc::ServerContext*, const ModelOperateRequest*, ModelOperateResponse*) override;
  ::grpc::Status deleteModelVersion(::grpc::ServerContext*, const ModelOperateRequest*, ModelOperateResponse*) override;
  ::grpc::Status activateModel(::grpc::ServerContext*, const ModelOperateRequest*, ModelOperateResponse*) override;

private:
  USE_ROLE(ModelOperateImpl);
};
}  // namespace serving
}  // namespace adlik

#endif
