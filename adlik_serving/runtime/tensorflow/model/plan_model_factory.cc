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

struct PlanModelFactory::BatchPlanModel : private MetaGraph, private BatchingSession, BasePlanModel {
  BatchPlanModel(UniqueModel& bundle, BatchingScheduler& scheduler, const BatchingParameters& config)
      : MetaGraph(bundle->meta_graph_def),
        BatchingSession(*bundle->session),
        BasePlanModel(bundle),
        scheduler(scheduler),
        config(config) {
  }

private:
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
    return new BatchPlanModel(bundle, ROLE(BatchingScheduler), ROLE(BatchingParameters));
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
