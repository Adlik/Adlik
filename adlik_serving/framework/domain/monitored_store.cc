// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/monitored_store.h"

namespace adlik {
namespace serving {

void MonitoredStore::update(const MonitoredEvent& event) {
  models[event.name()].update(event);
}

void MonitoredStore::deleteModel(const std::string& modelName){
  auto it = models.find(modelName);
  models.erase(it);
}

bool MonitoredStore::ready(const std::string& name, const MonitoredPredicate& pred) const {
  for (auto& p : models) {
    if (p.first == name) {
      return p.second.ready(pred);
    }
  }
  return false;
}

}  // namespace serving
}  // namespace adlik
