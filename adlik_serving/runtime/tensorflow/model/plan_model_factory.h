// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HB3D6BA8F_B9C5_49CA_AA4B_EFE318E6BB0F
#define HB3D6BA8F_B9C5_49CA_AA4B_EFE318E6BB0F

#include <memory>

#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/domain/model_id.h"
#include "cub/dci/role.h"
#include "tensorflow/cc/saved_model/loader.h"

namespace tensorflow {

struct PlanModel;
struct BatchingParameters;
struct TfPlanModelConfig;
struct BatchingScheduler;

DEFINE_ROLE(PlanModelFactory) {
  PlanModel* create();

private:
  using UniqueModel = std::unique_ptr<SavedModelBundle>;

  struct BasePlanModel;
  struct AtomPlanModel;
  struct BatchPlanModel;

  PlanModel* make(UniqueModel&);
  std::string getModelPath() const;

private:
  USE_ROLE(BatchingScheduler);
  USE_ROLE(BatchingParameters);
  USE_ROLE(TfPlanModelConfig);
  USE_ROLE_NS(adlik::serving, ModelId);
  USE_ROLE_NS(adlik::serving, ModelConfig);
};

}  // namespace tensorflow

#endif
