// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H3DFDF919_D321_4ECB_8BC1_43733F9485FC
#define H3DFDF919_D321_4ECB_8BC1_43733F9485FC

#include <memory>

#include "adlik_serving/framework/domain/bare_model_loader.h"
#include "adlik_serving/framework/domain/model_handle.h"
#include "adlik_serving/runtime/tensorflow/model/plan_model.h"

namespace tensorflow {

struct PlanModelFactory;

struct PlanModelLoader : adlik::serving::BareModelLoader, adlik::serving::ModelHandle {
private:
  OVERRIDE(cub::Status loadBare());
  OVERRIDE(cub::Status unloadBare());

private:
  OVERRIDE(cub::AnyPtr model());

private:
  std::unique_ptr<PlanModel> bundle;

private:
  USE_ROLE(PlanModelFactory);
};  // namespace tensorflow

}  // namespace tensorflow

#endif
