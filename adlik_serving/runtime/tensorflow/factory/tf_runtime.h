// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HCA512D18_ED0E_4BF5_892B_2F056F5DB367
#define HCA512D18_ED0E_4BF5_892B_2F056F5DB367

#include <string>

#include "adlik_serving/framework/manager/runtime.h"
#include "adlik_serving/runtime/tensorflow/batch/batching_parameters.h"
#include "adlik_serving/runtime/tensorflow/batch/batching_scheduler.h"
#include "adlik_serving/runtime/tensorflow/factory/tf_model_factory.h"
#include "adlik_serving/runtime/tensorflow/factory/tf_plan_model_config.h"
#include "adlik_serving/runtime/tensorflow/factory/tf_plan_model_options.h"

namespace tensorflow {

struct TfRuntime : adlik::serving::Runtime,
                   private TfPlanModelOptions,
                   private TfModelFactory,
                   private TfPlanModelConfig,
                   private BatchingScheduler,
                   private BatchingParameters {
  TfRuntime(const std::string&);

private:
  OVERRIDE(void init(cub::ProgramOptions&));
  OVERRIDE(cub::Status config(const adlik::serving::RuntimeContext&));
  OVERRIDE(Status predict(const adlik::serving::RunOptions&,
                          adlik::serving::ModelHandle*,
                          const adlik::serving::PredictRequest&,
                          adlik::serving::PredictResponse&));

private:
  IMPL_ROLE(BatchingScheduler)
  IMPL_ROLE(BatchingParameters)
  IMPL_ROLE(TfPlanModelConfig)

  std::string runtime_name;
};

}  // namespace tensorflow

#endif
