// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/apis/task_op_impl.h"

#include "adlik_serving/apis/task.pb.h"
#include "adlik_serving/framework/domain/model_store.h"
#include "adlik_serving/framework/manager/runtime_suite.h"
#include "adlik_serving/framework/manager/serving_store.h"
#include "cub/log/log.h"

namespace adlik {
namespace serving {

cub::StatusWrapper TaskOpImpl::create(const RunOptions& options,
                                      const ::google::protobuf::Any& req,
                                      ::google::protobuf::Any& rsp) {
  DEBUG_LOG << "Receive create task request";
  if (!req.Is<::adlik::serving::CreateTaskRequest>()) {
    return cub::StatusWrapper(cub::InvalidArgument, "Input is not CreateTaskRequest!");
  }

  adlik::serving::CreateTaskRequest task_request;
  if (!req.UnpackTo(&task_request)) {
    return cub::StatusWrapper(cub::InvalidArgument, "UnpackTo CreateTaskRequest failed!");
  }

  std::unique_ptr<ModelHandle> handle = ROLE(ServingStore).find(task_request.model_spec());
  auto config = ROLE(ModelStore).find(task_request.model_spec().name());
  if (handle && config) {
    if (auto runtime = RuntimeSuite::inst().get(config->platform())) {
      adlik::serving::CreateTaskResponse task_response;
      auto status = runtime->createTask(options, handle.get(), task_request, task_response);
      rsp.PackFrom(task_response);
      return status;
    }
  }
  return cub::StatusWrapper(cub::InvalidArgument, "Not found model");
}

}  // namespace serving
}  // namespace adlik
