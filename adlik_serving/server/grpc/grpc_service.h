// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_SERVER_GRPC_GRPC_SERVICE_H
#define ADLIK_SERVING_SERVER_GRPC_GRPC_SERVICE_H

#include "adlik_serving/apis/predict_service.grpc.pb.h"
#include "cub/base/status.h"
#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct PredictImpl;
struct GetModelMetaImpl;

struct GrpcService : PredictService::Service {
private:
  ::grpc::Status predict(::grpc::ServerContext*, const PredictRequest*, PredictResponse*) override;

  ::grpc::Status getModelMeta(::grpc::ServerContext*, const GetModelMetaRequest*, GetModelMetaResponse*) override;

private:
  USE_ROLE(PredictImpl);
  USE_ROLE(GetModelMetaImpl);
};
}  // namespace serving
}  // namespace adlik

#endif
