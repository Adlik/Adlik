#ifndef ADLIK_SERVING_RUNTIME_UTIL_DEFAULT_RUNTIME_HELPER_H
#define ADLIK_SERVING_RUNTIME_UTIL_DEFAULT_RUNTIME_HELPER_H

#include <string>

#include "adlik_serving/framework/domain/auto_model_handle.h"
#include "adlik_serving/framework/manager/auto_runtime_register.h"
#include "adlik_serving/framework/manager/model_factory.h"
#include "adlik_serving/framework/manager/model_factory_suite.h"
#include "adlik_serving/framework/manager/runtime.h"
#include "adlik_serving/framework/manager/runtime_context.h"
#include "adlik_serving/runtime/provider/predict_util.h"
#include "adlik_serving/runtime/util/default_model_factory.h"
#include "adlik_serving/runtime/util/default_model_wrapper.h"
#include "adlik_serving/runtime/util/name_def.h"

namespace adlik {
namespace serving {

template <typename Wrapper, typename ModelType>
struct DefaultRuntime : Runtime, DefaultModelFactory<Wrapper> {
  DefaultRuntime(const std::string& name) : runtime_name(name) {
  }

private:
  using MyFactory = DefaultModelFactory<Wrapper>;
  OVERRIDE(cub::Status config(const RuntimeContext& ctxt)) {
    SELF(MyFactory).config();
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

#define DEFINE_RUNTIME_HELPER(Name, Wrapper, ModelType)                                \
  using RUNTIME(Name) = adlik::serving::DefaultRuntime<Wrapper<ModelType>, ModelType>; \
  REGISTER_RUNTIME(RUNTIME(Name), #Name)

#define DEFINE_DEFAULT_RUNTIME(Name, ModelType) \
  DEFINE_RUNTIME_HELPER(Name, adlik::serving::DefaultModelWrapper, ModelType)

}  // namespace serving
}  // namespace adlik

#endif
