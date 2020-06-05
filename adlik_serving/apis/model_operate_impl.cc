// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/apis/model_operate_impl.h"

#include "adlik_serving/apis/model_operate.pb.h"
#include "adlik_serving/framework/domain/model_options.h"
#include "adlik_serving/framework/domain/model_store.h"
#include "adlik_serving/framework/domain/state_monitor.h"
#include "adlik_serving/framework/manager/boarding_loop.h"
#include "adlik_serving/framework/manager/managed_store.h"
#include "adlik_serving/framework/manager/runtime_suite.h"
#include "adlik_serving/framework/manager/serving_store.h"
#include "adlik_serving/framework/manager/storage_loop.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"
#include "cub/string/str_utils.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {
namespace {
void SuccessResponse(ModelOperateResponse& rsp) {
  rsp.set_status("SUCCESS");
}

void FailureResponse(ModelOperateResponse& rsp, std::string error_message) {
  rsp.set_status("ERROR");
  rsp.set_error_message(error_message);
}

struct MaxVersionSelector : cub::DirentVisitor {
  int maxVersion() const {
    return versions.max();
  }

private:
  OVERRIDE(void visit(const std::string&, const std::string& child)) {
    uint32_t version = 1;
    if (cub::strutils::to_uint32(child, version)) {
      versions.add(version);
    }
  }

private:
  VersionList versions;
};
}  // namespace

tensorflow::Status ModelOperateImpl::addModel(const ModelOperateRequest& req, ModelOperateResponse& rsp) {
  std::string modelName = req.model_name();
  std::string sourcePath = req.path();
  INFO_LOG << "add model " << modelName;
  tensorflow::Status status = tensorflow::Status::OK();
  if (ROLE(ModelStore).exist(modelName)) {
    INFO_LOG << modelName << " model is already exist";
    FailureResponse(rsp, modelName + " model is already exist");
    return status;
  }
  std::string targetPath = cub::paths(ROLE(ModelOptions).getBasePath(), modelName);
  if (cub::isFailStatus(cub::filesystem().copyDir(sourcePath, targetPath))) {
    FailureResponse(rsp, modelName + " copy model path error");
    return status;
  }
  auto operateFailure = [&rsp, &status, &modelName, &targetPath, this](std::string error_message) {
    this->ROLE(ModelStore).deleteModel(modelName);
    this->ROLE(ManagedStore).deleteModel(modelName);
    cub::filesystem().deleteDir(targetPath);
    FailureResponse(rsp, modelName + " " + error_message);
    return status;
  };
  if (cub::isFailStatus(ROLE(ModelStore).configModel(modelName)) || !ROLE(ModelStore).exist(modelName)) {
    return operateFailure("config error");
  }
  auto config = ROLE(ModelStore).find(modelName);
  if (!RuntimeSuite::inst().get(config->platform())) {
    return operateFailure("config runtime error");
  }
  ROLE(StorageLoop).once();
  ROLE(BoardingLoop).once();
  if (!ROLE(ManagedStore).exist(modelName) || !ROLE(ManagedStore).isNormal(modelName)) {
    operateFailure("start up model error");
  }
  return status;
  SuccessResponse(rsp);
  return status;
}

tensorflow::Status ModelOperateImpl::deleteModel(const ModelOperateRequest& req, ModelOperateResponse& rsp) {
  std::string modelName = req.model_name();
  INFO_LOG << "delete model " << modelName;
  tensorflow::Status status = tensorflow::Status::OK();
  auto config = ROLE(ModelStore).find(modelName);
  if (!config) {
    INFO_LOG << modelName << " model is not exist";
    FailureResponse(rsp, modelName + " model is not exist");
    return status;
  }
  std::string targetPath = config->getBasePath();
  ROLE(ModelStore).deleteModel(modelName);
  ROLE(ManagedStore).deleteModel(modelName);
  if (cub::isFailStatus(cub::filesystem().deleteDir(targetPath))) {
    FailureResponse(rsp, modelName + " model delete failure");
    return status;
  }
  SuccessResponse(rsp);
  return status;
}

tensorflow::Status ModelOperateImpl::addModelVersion(const ModelOperateRequest& req, ModelOperateResponse& rsp) {
  std::string modelName = req.model_name();
  INFO_LOG << "add model version " << modelName;
  std::string sourcePath = req.path();
  tensorflow::Status status = tensorflow::Status::OK();
  auto config = ROLE(ModelStore).find(modelName);
  if (!config) {
    INFO_LOG << modelName << " model is not exist";
    FailureResponse(rsp, modelName + " model is not exist");
    return status;
  }
  std::string modelPath = config->getBasePath();
  MaxVersionSelector maxVersionSelector;
  cub::filesystem().children(modelPath, maxVersionSelector);
  std::string targetPath = cub::paths(modelPath, std::to_string(maxVersionSelector.maxVersion() + 1));
  if (cub::isFailStatus(cub::filesystem().copyDir(sourcePath, targetPath))) {
    FailureResponse(rsp, modelName + " copy model version path error");
    return status;
  }
  ROLE(StorageLoop).once();
  ROLE(BoardingLoop).once();
  if (!ROLE(ManagedStore).exist(modelName) || !ROLE(ManagedStore).isNormal(modelName)) {
    cub::filesystem().deleteDir(targetPath);
    ROLE(StorageLoop).once();
    ROLE(BoardingLoop).once();
    FailureResponse(rsp, modelName + " model version start up error");
  }
  SuccessResponse(rsp);
  return status;
}

tensorflow::Status ModelOperateImpl::deleteModelVersion(const ModelOperateRequest& req, ModelOperateResponse& rsp) {
  std::string modelName = req.model_name();
  std::string modelVersion = req.model_version();
  INFO_LOG << "delete model " << modelName << " version " << modelVersion;
  tensorflow::Status status = tensorflow::Status::OK();
  auto config = ROLE(ModelStore).find(modelName);
  if (!config) {
    INFO_LOG << modelName << " model is not exist";
    FailureResponse(rsp, modelName + " model is not exist");
    return status;
  }
  ModelId id(modelName, cub::strutils::to_int32(modelVersion));
  std::string targetPath = config->getModelPath(id);
  if (cub::isFailStatus(cub::filesystem().deleteDir(targetPath))) {
    FailureResponse(rsp, modelName + " model version delete failure");
    return status;
  }
  ROLE(StorageLoop).once();
  ROLE(BoardingLoop).once();
  SuccessResponse(rsp);
  return status;
}
}  // namespace serving
}  // namespace adlik
