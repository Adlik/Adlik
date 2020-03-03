// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/grpc/task_op_service.h"

#include "adlik_serving/apis/task_op_impl.h"
#include "adlik_serving/framework/manager/run_options.h"
#include "grpc/grpc.h"

namespace adlik {
namespace serving {

namespace {

::grpc::Status toGrpcStatus(const cub::StatusWrapper& status) {
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

::grpc::Status TaskOpService::create(::grpc::ServerContext* ctxt,
                                     const CreateTaskRequest* req,
                                     CreateTaskResponse* rsp) {
  auto status = ROLE(TaskOpImpl).create(options(*ctxt), *req, *rsp);
  return toGrpcStatus(status);
}

}  // namespace serving
}  // namespace adlik
