// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_APIS_MODEL_OPERATE_IMPL_H
#define ADLIK_SERVING_APIS_MODEL_OPERATE_IMPL_H

#include <string>

#include "cub/dci/role.h"
#include "tensorflow/core/lib/core/status.h"

namespace adlik {
namespace serving {

struct ServingStore;
struct ModelStore;
struct ManagedStore;
struct ModelOptions;
struct StorageLoop;
struct BoardingLoop;
struct StateMonitor;
struct ModelOperateRequest;
struct ModelOperateResponse;
struct VersionPolicyProto;

struct ModelOperateImpl {
  virtual ~ModelOperateImpl() = default;

  tensorflow::Status addModel(const ModelOperateRequest&, ModelOperateResponse&);
  tensorflow::Status addModelVersion(const ModelOperateRequest&, ModelOperateResponse&);
  tensorflow::Status deleteModel(const ModelOperateRequest&, ModelOperateResponse&);
  tensorflow::Status deleteModelVersion(const ModelOperateRequest&, ModelOperateResponse&);
  tensorflow::Status activateModel(const ModelOperateRequest&, ModelOperateResponse&);
  tensorflow::Status queryModel(const ModelOperateRequest&, ModelOperateResponse&) const;

private:
  void update();
  bool versionLoad(const VersionPolicyProto& originPolicy, const std::string& modelName, const int version);
  void oldVersionUnload(const std::string& modelName, const int version);
  USE_ROLE(ServingStore);
  USE_ROLE(ModelStore);
  USE_ROLE(ManagedStore);
  USE_ROLE(ModelOptions);
  USE_ROLE(StateMonitor);
  USE_ROLE(StorageLoop);
  USE_ROLE(BoardingLoop);
};
}  // namespace serving
}  // namespace adlik

#endif
