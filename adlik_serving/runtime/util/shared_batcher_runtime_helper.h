#ifndef ADLIK_SERVING_RUNTIME_UTIL_SHARED_BATCHER_RUNTIME_HELPER_H
#define ADLIK_SERVING_RUNTIME_UTIL_SHARED_BATCHER_RUNTIME_HELPER_H

#include <string>

#include "adlik_serving/framework/manager/auto_runtime_register.h"
#include "adlik_serving/framework/manager/model_factory_suite.h"
#include "adlik_serving/framework/manager/runtime.h"
#include "adlik_serving/framework/manager/runtime_context.h"
#include "adlik_serving/runtime/provider/predict_util.h"
#include "adlik_serving/runtime/util/name_def.h"
#include "adlik_serving/runtime/util/shared_batcher_model_factory.h"
#include "adlik_serving/runtime/util/shared_batcher_model_wrapper.h"
#include "adlik_serving/runtime/util/shared_batcher_wrapper.h"
#include "cub/base/assertions.h"

namespace adlik {
namespace serving {

template <typename ModelType>
struct SharedBatcherRuntime : Runtime,
                              SharedBatcherModelFactory<SharedBatcherModelWrapper<ModelType>>,
                              SharedBatcherWrapper {
  SharedBatcherRuntime(const std::string& name) : runtime_name(name) {
  }

private:
  IMPL_ROLE(SharedBatcherWrapper)

  using MyFactory = SharedBatcherModelFactory<SharedBatcherModelWrapper<ModelType>>;
  OVERRIDE(cub::Status config(const RuntimeContext& ctxt)) {
    SELF(MyFactory).config();
    SELF(SharedBatcherWrapper).config(runtime_name);
    ctxt.ROLE(ModelFactorySuite).add(runtime_name, *this);
    return cub::Success;
  }

  OVERRIDE(tensorflow::Status predict(const RunOptions& opts,
                                      ModelHandle* handle,
                                      const PredictRequest& req,
                                      PredictResponse& rsp)) {
    return PredictUtil(opts, handle, req, rsp).predict<ModelType>();
  }

  std::string runtime_name;
};

#define DEFINE_SHARED_BATCHER_RUNTIME(Name, ModelType)                   \
  using RUNTIME(Name) = adlik::serving::SharedBatcherRuntime<ModelType>; \
  REGISTER_RUNTIME(RUNTIME(Name), #Name)

}  // namespace serving
}  // namespace adlik

#endif
