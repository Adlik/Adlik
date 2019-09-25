#include "adlik_serving/framework/manager/managed_store.h"

#include "adlik_serving/framework/domain/event_bus.h"
#include "adlik_serving/framework/domain/model_store.h"
#include "adlik_serving/framework/manager/model_factory.h"
#include "adlik_serving/framework/manager/serving_store.h"
#include "cub/base/assertions.h"

namespace adlik {
namespace serving {

template <typename F>
inline void ManagedStore::select(const std::string& name, F f) const {
  auto range = manageds.equal_range(name);
  for (auto it = range.first; it != range.second; ++it) {
    f(it);
  }
}

auto ManagedStore::find(const ModelId& id) const -> decltype(manageds.cend()) {
  auto range = manageds.equal_range(id.getName());
  for (auto i = range.first; i != range.second; ++i) {
    if (i->second->getVersion() == id.getVersion())
      return i;
  }
  return manageds.cend();
}

template <typename F>
cub::Status ManagedStore::iterator(const ModelId& id, F f) {
  auto i = find(id);
  CUB_ASSERT_TRUE(i != manageds.cend());
  CUB_ASSERT_SUCC_CALL(f(i));
  return cub::Success;
}

template <typename F>
cub::Status ManagedStore::process(const ModelId& id, F f) {
  return iterator(id, [f](auto& i) { return f(*i->second); });
}

void ManagedStore::names(ManagedNameVisitor& visitor) const {
  for (auto i = manageds.begin(); i != manageds.end();) {
    visitor.visit(i->first);
    i = manageds.equal_range(i->first).second;
  }
}

void ManagedStore::models(ManagedModelVisitor& visitor) const {
  for (auto& m : manageds) {
    visitor.visit(*m.second.get());
  }
}

void ManagedStore::models(const std::string& name, ManagedModelVisitor& visitor) const {
  select(name, [&visitor](auto i) { visitor.visit(*i->second.get()); });
}

void ManagedStore::publish(const ModelId& id) const {
  auto i = find(id);
  if (i != manageds.cend()) {
    ROLE(EventBus).publish(*i->second);
  }
}

inline cub::Status ManagedStore::onStartSucc(const ModelId& id, Model* model) {
  INFO_LOG << "starting model " << id.to_s();
  manageds.emplace(id.getName(), SharedModel(model));
  publish(id);
  return cub::Success;
}

inline cub::Status ManagedStore::onStartFail(const ModelId& id, const char* msg) {
  INFO_LOG << "start model fail: " << msg << id.to_s();
  return cub::Unavailable;
}

inline cub::Status ManagedStore::doStartup(const ModelId& id, const ModelConfig& config) {
  auto model = ROLE(ModelFactory).create(id, config);
  return model ? onStartSucc(id, model) : onStartFail(id, "create model fail");
}

inline cub::Status ManagedStore::startup(const ModelId& id) {
  INFO_LOG << "startup " << id.to_s();
  auto config = ROLE(ModelStore).find(id.getName());
  return config ? doStartup(id, *config) : onStartFail(id, "not found model config");
}

cub::Status ManagedStore::start(const ModelId& id) {
  auto i = find(id);
  return i == manageds.cend() ? startup(id) : onStartFail(id, "model was existed");
}

cub::Status ManagedStore::stop(const ModelId& id) {
  auto releasing = [this, &id](auto& iter) {
    INFO_LOG << "stop model" << id.to_s();
    manageds.erase(iter);
    return cub::Success;
  };
  return iterator(id, releasing);
}

template <typename F>
cub::Status ManagedStore::boarding(const ModelId& id, F f) {
  CUB_ASSERT_SUCC_CALL(process(id, f));
  ROLE(ServingStore).update(manageds);
  publish(id);
  return cub::Success;
}

cub::Status ManagedStore::load(const ModelId& id) {
  auto loading = [&id](ModelLoader& loader) {
    INFO_LOG << "loading model" << id.to_s();
    return loader.load();
  };
  return boarding(id, loading);
}

cub::Status ManagedStore::unload(const ModelId& id) {
  auto unloading = [&id](ModelLoader& loader) {
    INFO_LOG << "unloading model" << id.to_s();
    return loader.unload();
  };
  return boarding(id, unloading);
}

cub::Status ManagedStore::unaspired(const ModelId& id) {
  auto unaspiring = [&id](AspiredState& state) {
    INFO_LOG << "unaspired model" << id.to_s();
    state.unaspired();
    return cub::Success;
  };
  return process(id, unaspiring);
}

}  // namespace serving
}  // namespace adlik
