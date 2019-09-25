#ifndef ADLIK_SERVING_APIS_GET_MODEL_META_IMPL_H
#define ADLIK_SERVING_APIS_GET_MODEL_META_IMPL_H

#include <string>

#include "cub/dci/role.h"
#include "tensorflow/core/lib/core/status.h"

namespace adlik {
namespace serving {

struct ServingStore;
struct ModelStore;
struct ManagedStore;
struct GetModelMetaRequest;
struct GetModelMetaResponse;
struct ModelConfig;

struct GetModelMetaImpl {
  virtual ~GetModelMetaImpl() = default;

  tensorflow::Status getModelMeta(const GetModelMetaRequest&, GetModelMetaResponse&);
  const ModelConfig* getModelConfig(const std::string& name) const;

private:
  USE_ROLE(ServingStore);
  USE_ROLE(ModelStore);
  USE_ROLE(ManagedStore);
};
}  // namespace serving
}  // namespace adlik

#endif
