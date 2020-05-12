// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/apis/model_operate_impl.h"

#include "adlik_serving/apis/model_operate.pb.h"
#include "adlik_serving/framework/domain/model_store.h"
#include "adlik_serving/framework/domain/state_monitor.h"
#include "adlik_serving/framework/manager/managed_store.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

tensorflow::Status ModelOperateImpl::addModel(const ModelOperateRequest& req, ModelOperateResponse& rsp) {
  cub::Status status = ROLE(ManagedStore).addModel(req.model_name(), req.path());
  if (cub::isSuccStatus(status)) {
    rsp.set_status("SUCCESS");
  } else {
    rsp.set_status("ERROR");
  }
  return tensorflow::Status::OK();
}

tensorflow::Status ModelOperateImpl::deleteModel(const ModelOperateRequest& req, ModelOperateResponse& rsp) {
  cub::Status status = ROLE(ManagedStore).deleteModel(req.model_name());
  if (cub::isSuccStatus(status)) {
    rsp.set_status("SUCCESS");
  } else {
    rsp.set_status("ERROR");
  }
  return tensorflow::Status::OK();
}

}  // namespace serving
}  // namespace adlik
