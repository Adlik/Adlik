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
                                      const CreateTaskRequest& req,
                                      CreateTaskResponse& rsp) {
  DEBUG_LOG << "Receive create task request";
  std::unique_ptr<ModelHandle> handle = ROLE(ServingStore).find(req.model_spec());
  auto config = ROLE(ModelStore).find(req.model_spec().name());
  if (handle && config) {
    if (auto runtime = RuntimeSuite::inst().get(config->platform())) {
      return runtime->createTask(options, handle.get(), req, rsp);
    }
  }
  return cub::StatusWrapper(cub::InvalidArgument, "Not found model");
}

}  // namespace serving
}  // namespace adlik
