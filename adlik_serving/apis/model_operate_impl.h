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
struct StateMonitor;
struct ModelConfig;
struct ModelOperateRequest;
struct ModelOperateResponse;

struct ModelOperateImpl {
  virtual ~ModelOperateImpl() = default;

  tensorflow::Status addModel(const ModelOperateRequest&, ModelOperateResponse&);
  tensorflow::Status deleteModel(const ModelOperateRequest&, ModelOperateResponse&);

private:
  USE_ROLE(ServingStore);
  USE_ROLE(ModelStore);
  USE_ROLE(ManagedStore);
  USE_ROLE(StateMonitor);
};
}  // namespace serving
}  // namespace adlik

#endif
