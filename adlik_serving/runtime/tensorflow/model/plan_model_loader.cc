// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/model/plan_model_loader.h"

#include "adlik_serving/runtime/tensorflow/model/plan_model_factory.h"

namespace tensorflow {

cub::Status PlanModelLoader::loadBare() {
  if (auto model = ROLE(PlanModelFactory).create()) {
    bundle.reset(model);
    return cub::Success;
  }
  return cub::Failure;
}

cub::Status PlanModelLoader::unloadBare() {
  bundle.reset();
  return cub::Success;
}

cub::AnyPtr PlanModelLoader::model() {
  return {bundle.get()};
}

}  // namespace tensorflow
