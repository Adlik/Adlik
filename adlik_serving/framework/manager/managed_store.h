// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H6F9C5D9E_C9D6_4C66_922B_1438E9301D94
#define H6F9C5D9E_C9D6_4C66_922B_1438E9301D94

#include <memory>
#include <unordered_map>
#include <vector>

#include "adlik_serving/framework/domain/shared_model.h"

namespace adlik {
namespace serving {

DEFINE_ROLE(ManagedModelVisitor) {
  ABSTRACT(void visit(const Model&));
};

DEFINE_ROLE(ManagedNameVisitor) {
  ABSTRACT(void visit(const std::string&));
};

struct EventBus;
struct ModelFactory;
struct ModelStore;
struct ModelConfig;
struct ServingStore;

DEFINE_ROLE(ManagedStore) {
  cub::Status start(const ModelId&);
  cub::Status stop(const ModelId&);

  cub::Status load(const ModelId&);
  cub::Status unload(const ModelId&);

  cub::Status unaspired(const ModelId&);
  bool exist(const std::string& modelName);
  bool isNormal(const std::string& modelName);
  void getModelVersions(const std::string& modelName, std::vector<int>& versions);
  void deleteModel(const std::string& modelName);
  void deleteModel(const ModelId&);
  void names(ManagedNameVisitor&) const;
  void models(ManagedModelVisitor&) const;
  void models(const std::string& name, ManagedModelVisitor&) const;

private:
  using ManagedModels = std::unordered_multimap<std::string, SharedModel>;
  ManagedModels::const_iterator find(const ModelId& id) const;

  template <typename F>
  cub::Status iterator(const ModelId&, F f);

  template <typename F>
  cub::Status process(const ModelId&, F f);

  template <typename F>
  void select(const std::string&, F f) const;

  template <typename F>
  cub::Status boarding(const ModelId&, F f);

  void publish(const ModelId& id) const;

  cub::Status doStartup(const ModelId&, const ModelConfig&);
  cub::Status startup(const ModelId&);
  cub::Status onStartSucc(const ModelId&, Model*);
  cub::Status onStartFail(const ModelId&, const char*);

private:
  ManagedModels manageds;

private:
  USE_ROLE(EventBus);
  USE_ROLE(ModelFactory);
  USE_ROLE(ModelStore);
  USE_ROLE(ServingStore);
};

}  // namespace serving
}  // namespace adlik

#endif
