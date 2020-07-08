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

struct VersionDirectoryState : cub::DirentVisitor {
  int maxVersion() const {
    return versions.max();
  }

  bool contains(int version) {
    return versions.contains(version);
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

struct QueryModelVerison : cub::DirentVisitor {
  QueryModelVerison(ModelOperateResponse& rsp, std::string& modelName) : rsp(rsp), modelName(modelName) {
  }

private:
  OVERRIDE(void visit(const std::string& base, const std::string& child)) {
    uint32_t version_no = 1;
    std::string path = cub::paths(base, child);
    if (cub::strutils::to_uint32(child, version_no) && cub::filesystem().isDir(path)) {
      auto version = (*rsp.mutable_models())[modelName].add_versions();
      version->set_version_no(version_no);
      version->set_state("OFFLINE");
      version->set_path(path);
    }
  }

private:
  ModelOperateResponse& rsp;
  std::string& modelName;
};

struct QueryConfig : ModelConfigVisitor {
  QueryConfig(ModelOperateResponse& rsp) : rsp(rsp) {
  }

private:
  OVERRIDE(void visit(const ModelConfig& model)) {
    std::string modelPath = model.getBasePath();
    std::string modelName = model.getModelName();
    (*rsp.mutable_models())[modelName].set_model_path(modelPath);
    QueryModelVerison queryModelVerison(rsp, modelName);
    cub::filesystem().children(modelPath, queryModelVerison);
  }

private:
  ModelOperateResponse& rsp;
};

struct QueryModelState : ManagedModelVisitor {
  QueryModelState(ModelOperateResponse& rsp) : rsp(rsp) {
  }

private:
  void visit(const Model& model) override {
    auto versions = (*rsp.mutable_models())[model.getName()].mutable_versions();
    for (auto iter = versions->begin(); iter != versions->end(); iter++) {
      if (model.getVersion() == iter->version_no()) {
        iter->set_state(model.str());
        break;
      }
    }
  }
  ModelOperateResponse& rsp;
};

bool versionExistInPath(const std::string& path, const int version) {
  VersionDirectoryState versionDirectoryState;
  cub::filesystem().children(path, versionDirectoryState);
  return versionDirectoryState.contains(version);
}

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
  update();
  if (!ROLE(ManagedStore).exist(modelName) || !ROLE(ManagedStore).isNormal(modelName)) {
    return operateFailure("start up model error");
  }
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
  VersionDirectoryState versionDirectoryState;
  cub::filesystem().children(modelPath, versionDirectoryState);
  std::string targetPath = cub::paths(modelPath, std::to_string(versionDirectoryState.maxVersion() + 1));
  if (cub::isFailStatus(cub::filesystem().copyDir(sourcePath, targetPath))) {
    FailureResponse(rsp, modelName + " copy model version path error");
    return status;
  }
  update();
  if (!ROLE(ManagedStore).exist(modelName) || !ROLE(ManagedStore).isNormal(modelName)) {
    cub::filesystem().deleteDir(targetPath);
    update();
    FailureResponse(rsp, modelName + " model version start up error");
    return status;
  }
  SuccessResponse(rsp);
  return status;
}

tensorflow::Status ModelOperateImpl::deleteModelVersion(const ModelOperateRequest& req, ModelOperateResponse& rsp) {
  std::string modelName = req.model_name();
  int modelVersion = req.model_version();
  INFO_LOG << "delete model " << modelName << " version " << modelVersion;
  tensorflow::Status status = tensorflow::Status::OK();
  auto config = ROLE(ModelStore).find(modelName);
  if (!config) {
    INFO_LOG << modelName << " model is not exist";
    FailureResponse(rsp, modelName + " model is not exist");
    return status;
  }
  ModelId id(modelName, modelVersion);
  std::string targetPath = config->getModelPath(id);
  if (cub::isFailStatus(cub::filesystem().deleteDir(targetPath))) {
    FailureResponse(rsp, modelName + " model version delete failure");
    return status;
  }
  update();
  SuccessResponse(rsp);
  return status;
}

tensorflow::Status ModelOperateImpl::activateModel(const ModelOperateRequest& req, ModelOperateResponse& rsp) {
  std::string modelName = req.model_name();
  int modelVersion = req.model_version();
  INFO_LOG << "activate model " << modelName << ", version " << std::to_string(modelVersion);
  tensorflow::Status status = tensorflow::Status::OK();
  auto config = ROLE(ModelStore).find(modelName);
  if (!config) {
    INFO_LOG << modelName << " model is not exist";
    FailureResponse(rsp, modelName + " model is not exist");
    return status;
  }
  if (!versionExistInPath(config->getBasePath(), modelVersion)) {
    FailureResponse(rsp, modelName + " model version " + std::to_string(modelVersion) + " is not exist");
    return status;
  }
  VersionPolicyProto originPolicy(config->version_policy());
  if (!versionLoad(originPolicy, modelName, modelVersion)) {
    FailureResponse(rsp, modelName + " model activate version start up error");
    return status;
  }
  oldVersionUnload(modelName, modelVersion);
  SuccessResponse(rsp);
  return status;
}

bool ModelOperateImpl::versionLoad(const VersionPolicyProto& originPolicy,
                                   const std::string& modelName,
                                   const int version) {
  VersionPolicyProto tempPolicy;
  std::vector<int> originVersions;
  ROLE(ManagedStore).getModelVersions(modelName, originVersions);
  for (auto originVersion : originVersions) {
    tempPolicy.mutable_specific()->add_versions(originVersion);
  }
  tempPolicy.mutable_specific()->add_versions(version);
  ROLE(ModelStore).updatePolicy(modelName, tempPolicy);
  update();
  if (!ROLE(ManagedStore).exist(modelName) || !ROLE(ManagedStore).isNormal(modelName)) {
    ROLE(ModelStore).updatePolicy(modelName, originPolicy);
    update();
    return false;
  }
  return true;
}

void ModelOperateImpl::oldVersionUnload(const std::string& modelName, const int resverdVersion) {
  VersionPolicyProto versionPolicy;
  versionPolicy.mutable_specific()->add_versions(resverdVersion);
  ROLE(ModelStore).updatePolicy(modelName, versionPolicy);
  update();
}

void ModelOperateImpl::update() {
  ROLE(StorageLoop).once();
  ROLE(BoardingLoop).once();
}

tensorflow::Status ModelOperateImpl::queryModel(const ModelOperateRequest& req, ModelOperateResponse& rsp) {
  std::string modelName = req.model_name();
  tensorflow::Status status = tensorflow::Status::OK();
  QueryConfig queryConfig(rsp);
  QueryModelState queryModelState(rsp);
  if (modelName != "") {
    INFO_LOG << "query model " << modelName << " status";
    if (!ROLE(ModelStore).exist(modelName)) {
      INFO_LOG << modelName << " model is not exist";
      FailureResponse(rsp, modelName + " model is not exist");
      return status;
    }
    ROLE(ModelStore).models(modelName, queryConfig);
    ROLE(ManagedStore).models(modelName, queryModelState);
  } else {
    INFO_LOG << "query all model status";
    ROLE(ModelStore).models(queryConfig);
    ROLE(ManagedStore).models(queryModelState);
  }
  SuccessResponse(rsp);
  return status;
}

}  // namespace serving
}  // namespace adlik
