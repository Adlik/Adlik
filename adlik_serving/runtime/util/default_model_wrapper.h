#ifndef ADLIK_SERVING_RUNTIME_UTIL_DEFAULT_MODEL_WRAPPER_H
#define ADLIK_SERVING_RUNTIME_UTIL_DEFAULT_MODEL_WRAPPER_H

#include <memory>

#include "adlik_serving/framework/domain/bare_model_loader.h"
#include "adlik_serving/framework/domain/model.h"
#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/domain/model_handle.h"

namespace adlik {
namespace serving {

template <typename T>
struct DefaultModelWrapper : private BareModelLoader, private ModelHandle, Model {
  DefaultModelWrapper(const ModelId& id, const ModelConfig& config) : Model(id), config(config) {
  }

private:
  OVERRIDE(cub::Status loadBare()) {
    return T::create(config, ROLE(ModelId), &model_bundle);
  }

  OVERRIDE(cub::Status unloadBare()) {
    model_bundle.reset();
    return cub::Success;
  }

  OVERRIDE(cub::AnyPtr model()) {
    return {model_bundle.get()};
  }

private:
  IMPL_ROLE(ModelHandle)
  IMPL_ROLE(ModelId)
  IMPL_ROLE(ModelState)
  IMPL_ROLE(BareModelLoader)

private:
  const ModelConfig& config;
  std::unique_ptr<T> model_bundle;
};

}  // namespace serving
}  // namespace adlik

#endif
