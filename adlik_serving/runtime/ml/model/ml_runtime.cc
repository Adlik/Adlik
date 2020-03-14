// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/auto_model_handle.h"
#include "adlik_serving/framework/manager/auto_runtime_register.h"
#include "adlik_serving/framework/manager/model_factory_suite.h"
#include "adlik_serving/framework/manager/runtime_context.h"
#include "adlik_serving/runtime/ml/model/ml_model.h"
#include "adlik_serving/runtime/util/default_model_factory.h"
#include "adlik_serving/runtime/util/default_model_wrapper.h"
#include "cub/dci/role.h"

namespace ml_runtime {

using namespace adlik::serving;

using MLWrapper = DefaultModelWrapper<MLModel>;

struct MLRuntime : Runtime, DefaultModelFactory<MLWrapper> {
  MLRuntime(const std::string& name) : runtime_name(name) {
  }

private:
  cub::StatusWrapper createTask(const RunOptions&,
                                ModelHandle* handle,
                                const CreateTaskRequest& req,
                                CreateTaskResponse& rsp) {
    AutoModelHandle<MLModel> bundle(handle);
    return bundle->run(req, rsp);
  }

  using MyFactory = DefaultModelFactory<MLWrapper>;
  OVERRIDE(cub::Status config(const RuntimeContext& ctxt)) {
    SELF(MyFactory).config();
    ctxt.ROLE(ModelFactorySuite).add(runtime_name, *this);
    return cub::Success;
  }

  std::string runtime_name;
};

REGISTER_RUNTIME(MLRuntime, "ml");

}  // namespace ml_runtime
