// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/model/plan_model_factory.h"

#include <memory>

#include "adlik_serving/runtime/tensorflow/batch/batching_parameters.h"
#include "adlik_serving/runtime/tensorflow/batch/batching_scheduler.h"
#include "adlik_serving/runtime/tensorflow/batch/batching_session.h"
#include "adlik_serving/runtime/tensorflow/factory/tf_plan_model_config.h"
#include "adlik_serving/runtime/tensorflow/model/meta_graph.h"
#include "adlik_serving/runtime/tensorflow/model/plan_model.h"
#include "cub/log/log.h"

namespace tensorflow {

struct PlanModelFactory::BasePlanModel : PlanModel {
  BasePlanModel(UniqueModel& bundle) : bundle(std::move(bundle)) {
  }

protected:
  UniqueModel bundle;

private:
  IMPL_ROLE_WITH_OBJ(MetaGraphDef, bundle->meta_graph_def)
};

struct PlanModelFactory::AtomPlanModel : BasePlanModel {
  using BasePlanModel::BasePlanModel;

private:
  IMPL_ROLE_WITH_OBJ(Session, *(bundle->session))
};

struct PlanModelFactory::BatchPlanModel : private MetaGraph, BatchingSession, BasePlanModel {
  BatchPlanModel(const adlik::serving::ModelConfig& config_proto,
                 UniqueModel& bundle,
                 BatchingScheduler& scheduler,
                 const BatchingParameters& config)
      : MetaGraph(bundle->meta_graph_def),
        BatchingSession(*bundle->session),
        BasePlanModel(bundle),
        config_proto(config_proto),
        scheduler(scheduler),
        config(config) {
  }

  void createSched() {
    BatchingSession::config(config_proto);
  }

private:
  const adlik::serving::ModelConfig& config_proto;
  BatchingScheduler& scheduler;
  const BatchingParameters& config;

private:
  IMPL_ROLE(Session)
  IMPL_ROLE(MetaGraph)
  IMPL_ROLE_WITH_OBJ(BatchingScheduler, scheduler)
  IMPL_ROLE_WITH_OBJ(BatchingParameters, config)
};

PlanModel* PlanModelFactory::make(UniqueModel& bundle) {
  if (ROLE(TfPlanModelConfig).batching()) {
    auto model = new BatchPlanModel(ROLE(ModelConfig), bundle, ROLE(BatchingScheduler), ROLE(BatchingParameters));
    model->createSched();
    return model;
  } else {
    return new AtomPlanModel(bundle);
  }
}

inline std::string PlanModelFactory::getModelPath() const {
  return ROLE(ModelConfig).getModelPath(ROLE(ModelId));
}

PlanModel* PlanModelFactory::create() {
  auto bundle = std::make_unique<SavedModelBundle>();
  auto status = ROLE(TfPlanModelConfig).load(getModelPath(), *bundle);
  INFO_LOG << "After load tensorflow model, status: " << status;
  return status.ok() ? make(bundle) : nullptr;
}

}  // namespace tensorflow
