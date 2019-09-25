#include "adlik_serving/framework/manager/model_factory_suite.h"

#include "adlik_serving/framework/domain/model_id.h"
#include "adlik_serving/framework/domain/model_store.h"
#include "cub/log/log.h"

namespace adlik {
namespace serving {

void ModelFactorySuite::add(const std::string& runtime, ModelFactory& f) {
  INFO_LOG << "register model factory:" << runtime;
  factories.insert({runtime, &f});
}

Model* ModelFactorySuite::create(const ModelId& id, const ModelConfig& config) {
  auto found = factories.find(config.platform());
  return found != factories.end() ? found->second->create(id, config) : nullptr;
}

}  // namespace serving
}  // namespace adlik
