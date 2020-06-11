// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_DEMO_PREDICT_SERVICE_H
#define ADLIK_SERVING_DEMO_PREDICT_SERVICE_H

#include "adlik_serving/apis/get_model_meta_impl.h"
#include "adlik_serving/apis/predict_impl.h"
#include "adlik_serving/framework/domain/event_bus.h"
#include "adlik_serving/framework/domain/model_options.h"
#include "adlik_serving/framework/domain/model_store.h"
#include "adlik_serving/framework/domain/state_monitor.h"
#include "adlik_serving/framework/manager/boarding_loop.h"
#include "adlik_serving/framework/manager/managed_store.h"
#include "adlik_serving/framework/manager/model_factory_suite.h"
#include "adlik_serving/framework/manager/runtime_context.h"
#include "adlik_serving/framework/manager/serving_store.h"
#include "adlik_serving/framework/manager/storage_loop.h"

namespace adlik {
namespace serving {
namespace demo {
class PredictService : ModelOptions,
                       ModelStore,
                       EventBus,
                       ModelFactorySuite,
                       ManagedStore,
                       ServingStore,
                       public PredictImpl,
                       public GetModelMetaImpl,
                       RuntimeContext,
                       StorageLoop,
                       BoardingLoop,
                       StateMonitor {
  IMPL_ROLE(ModelOptions)
  IMPL_ROLE(ServingStore)
  IMPL_ROLE(ModelStore)
  IMPL_ROLE(EventBus)
  IMPL_ROLE(ModelFactory)
  IMPL_ROLE(ManagedStore)
  IMPL_ROLE(ModelFactorySuite)

public:
  PredictService(const char* model_repository);

  void start();
};
}  // namespace demo
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_DEMO_PREDICT_SERVICE_H
