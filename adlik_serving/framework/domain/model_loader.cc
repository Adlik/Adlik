#include "adlik_serving/framework/domain/model_loader.h"
#include "adlik_serving/framework/domain/model_state.h"

namespace adlik {
namespace serving {

cub::Status ModelLoader::load() {
  return ROLE(ModelState).loadBy(ROLE(BareModelLoader));
}

cub::Status ModelLoader::unload() {
  return ROLE(ModelState).unloadBy(ROLE(BareModelLoader));
}

}  // namespace serving
}  // namespace adlik
