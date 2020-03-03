// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/factory/tf_model_factory.h"

#include "adlik_serving/framework/domain/model.h"
#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/runtime/tensorflow/model/plan_model_factory.h"
#include "adlik_serving/runtime/tensorflow/model/plan_model_loader.h"

using adlik::serving::Model;
using adlik::serving::ModelConfig;
using adlik::serving::ModelId;

namespace tensorflow {

namespace {
struct TfPlanModel : Model, private PlanModelLoader, private PlanModelFactory {
  TfPlanModel(const ModelId& id,
              const ModelConfig& config,
              const TfPlanModelConfig& tfconfig,
              const BatchingParameters& batching,
              BatchingScheduler& scheduler)
      : Model(id), config(config), tfconfig(tfconfig), batching(batching), scheduler(scheduler) {
  }

private:
  const adlik::serving::ModelConfig& config;
  const TfPlanModelConfig& tfconfig;
  const BatchingParameters& batching;
  BatchingScheduler& scheduler;

private:
  IMPL_ROLE(ModelId)
  IMPL_ROLE(ModelState)
  IMPL_ROLE(ModelHandle)
  IMPL_ROLE(BareModelLoader)

  IMPL_ROLE(PlanModelFactory)

  IMPL_ROLE_WITH_OBJ(TfPlanModelConfig, tfconfig)
  IMPL_ROLE_WITH_OBJ(BatchingParameters, batching)
  IMPL_ROLE_WITH_OBJ(BatchingScheduler, scheduler)
  IMPL_ROLE_WITH_OBJ(ModelConfig, config)
};
}  // namespace

Model* TfModelFactory::create(const ModelId& id, const ModelConfig& config) {
  return new TfPlanModel(id, config, ROLE(TfPlanModelConfig), ROLE(BatchingParameters), ROLE(BatchingScheduler));
}
}  // namespace tensorflow
