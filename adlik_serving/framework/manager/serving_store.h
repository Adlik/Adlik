#ifndef H4F031C2F_962D_44E1_BE35_47C34553E249
#define H4F031C2F_962D_44E1_BE35_47C34553E249

#include <memory>

#include "adlik_serving/framework/domain/model_handle.h"
#include "adlik_serving/framework/domain/model_request.h"
#include "adlik_serving/framework/domain/model_spec.pb.h"
#include "adlik_serving/framework/manager/model_map.h"
#include "cub/mem/read_write_ptr.h"

namespace adlik {
namespace serving {

struct ServingStore {
  void update(const ModelHashMap<std::string>&);
  std::unique_ptr<ModelHandle> find(const ModelSpec&) const;

private:
  struct SortedModels;
  using HashModels = ModelHashMap<ModelRequest>;

  ModelHandle* find(const ModelRequest&) const;

private:
  cub::ReadWritePtr<HashModels> models;
};

}  // namespace serving
}  // namespace adlik

#endif
