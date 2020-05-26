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
                                      const CreateTaskRequest& task_request,
                                      CreateTaskResponse& task_response) {
  DEBUG_LOG << "Receive create task request";

  task_response.mutable_model_spec()->CopyFrom(task_request.model_spec());
  task_response.set_task_type(task_request.task_type());

  std::unique_ptr<ModelHandle> handle = ROLE(ServingStore).find(task_request.model_spec());
  auto config = ROLE(ModelStore).find(task_request.model_spec().name());
  if (handle && config) {
    if (auto runtime = RuntimeSuite::inst().get(config->platform())) {
      auto status = runtime->createTask(options, handle.get(), task_request, task_response);
      return status;
    }
  }
  return cub::StatusWrapper(cub::InvalidArgument, "Not found model");
}

}  // namespace serving
}  // namespace adlik
