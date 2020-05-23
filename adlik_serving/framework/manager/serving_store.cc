// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/manager/serving_store.h"

#include "adlik_serving/framework/domain/model_request.h"
#include "adlik_serving/framework/domain/shared_model_handle.h"
#include "cub/log/log.h"

namespace adlik {
namespace serving {

std::unique_ptr<ModelHandle> ServingStore::find(const ModelSpec& spec) const {
  return std::unique_ptr<ModelHandle>(find(ModelRequest::from(spec)));
}

ModelHandle* ServingStore::find(const ModelRequest& req) const {
  auto handles = models.get();
  if (handles) {
    auto found = handles->find(req);
    if (found == handles->end()) {
      return nullptr;
    }
    return new SharedModelHandle(std::shared_ptr<ModelHandle>(handles, found->second->handle()));
  } else {
    return nullptr;
  }
}

struct ServingStore::SortedModels {
  SortedModels(const ModelHashMap<std::string>& manageds) {
    for (auto& i : manageds) {
      add(i.second);
    }
  }

  void appendTo(HashModels& dst) const {
    for (auto prev = models.end(), cur = models.begin(); cur != models.end(); prev = cur++) {
      appendTo(dst, prev, cur);
    }
  }

private:
  void add(const std::shared_ptr<Model>& model) {
    if (model->ready()) {
      models.emplace(ModelRequest::specific(*model), model);
    }
  }

  template <typename Iterator>
  void appendTo(HashModels& dst, Iterator prev, Iterator cur) const {
    specific(cur, dst, [](const ModelId& id) {
      INFO_LOG << "start serving specific model request: " << id.to_s();
      return ModelRequest::specific(id);
    });

    boundary(prev, cur, dst, [](auto& name) {
      INFO_LOG << "start serving earliest model request: " << name;
      return ModelRequest::earliest(name);
    });

    boundary(std::next(cur), cur, dst, [](auto& name) {
      INFO_LOG << "start serving latest model request: " << name;
      return ModelRequest::latest(name);
    });
  }

  template <typename Iterator>
  bool isBoundary(Iterator& pn, Iterator& cur) const {
    return pn == models.end() || name(pn) != name(cur);
  }

  template <typename Iterator, typename ModelRequest>
  void boundary(Iterator pn, Iterator& cur, HashModels& dst, ModelRequest request) const {
    if (isBoundary(pn, cur)) {
      dst.emplace(request(name(cur)), cur->second);
    }
  }

  template <typename Iterator, typename ModelRequest>
  void specific(Iterator& cur, HashModels& dst, ModelRequest request) const {
    dst.emplace(request(*cur->second), cur->second);
  }

  template <typename Iterator>
  const std::string& name(Iterator& iter) const {
    return iter->second->getName();
  }

private:
  ModelMap<ModelRequest> models;
};

void ServingStore::update(const ModelHashMap<std::string>& manageds) {
  auto dst = std::make_unique<HashModels>();
  SortedModels(manageds).appendTo(*dst);
  models.update(std::move(dst));
}

}  // namespace serving
}  // namespace adlik
