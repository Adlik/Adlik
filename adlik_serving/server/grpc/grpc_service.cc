// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/grpc/grpc_service.h"

#include "adlik_serving/apis/get_model_meta_impl.h"
#include "adlik_serving/apis/predict_impl.h"
#include "adlik_serving/framework/manager/run_options.h"
#include "adlik_serving/framework/manager/runtime_context.h"
#include "cub/log/log.h"
#include "grpc/grpc.h"

namespace adlik {
namespace serving {

namespace {
::grpc::Status toGrpcStatus(const tensorflow::Status& status) {
  return {static_cast<grpc::StatusCode>(status.code()), status.error_message()};
}

int deadline(const gpr_timespec& deadline) {
  return gpr_time_to_millis(
      gpr_time_sub(gpr_convert_clock_type(deadline, GPR_CLOCK_MONOTONIC), gpr_now(GPR_CLOCK_MONOTONIC)));
}

RunOptions options(const ::grpc::ServerContext& ctxt) {
  RunOptions opts;
  opts.timeout_in_ms = deadline(ctxt.raw_deadline());
  return opts;
}

}  // namespace

::grpc::Status GrpcService::predict(::grpc::ServerContext* ctxt, const PredictRequest* req, PredictResponse* rsp) {
  DEBUG_LOG << "Receive predict request, model_name: " << req->model_spec().name()
            << ", batch size: " << req->batch_size();

  auto status = ROLE(PredictImpl).predict(options(*ctxt), *req, *rsp);
  return toGrpcStatus(status);
}

::grpc::Status GrpcService::getModelMeta(::grpc::ServerContext*,
                                         const GetModelMetaRequest* req,
                                         GetModelMetaResponse* rsp) {
  return toGrpcStatus(ROLE(GetModelMetaImpl).getModelMeta(*req, *rsp));
}

}  // namespace serving
}  // namespace adlik
