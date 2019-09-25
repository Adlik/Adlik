#ifndef ADLIK_SERVING_RUNTIME_UTIL_SHARED_BATCHER_MODEL_FACTORY_H
#define ADLIK_SERVING_RUNTIME_UTIL_SHARED_BATCHER_MODEL_FACTORY_H

#include "adlik_serving/framework/manager/model_factory.h"
#include "cub/base/status.h"

namespace adlik {
namespace serving {

struct SharedBatcherWrapper;

template <typename ModelType>
struct SharedBatcherModelFactory : ModelFactory {
  void config() {
  }

  OVERRIDE(Model* create(const ModelId& id, const ModelConfig& model_config)) {
    return new ModelType(id, model_config, ROLE(SharedBatcherWrapper));
  }

private:
  USE_ROLE(SharedBatcherWrapper);
};

}  // namespace serving
}  // namespace adlik

#endif
