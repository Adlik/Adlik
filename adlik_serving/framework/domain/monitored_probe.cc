// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/monitored_probe.h"

#include "adlik_serving/framework/domain/monitored_event.h"
#include "adlik_serving/framework/domain/monitored_predicate.h"
#include "adlik_serving/framework/domain/monitored_store.h"

namespace adlik {
namespace serving {

ModelStateProbe::ModelStateProbe(const std::vector<std::string>& names, MonitoredNotifier notifier)
    : names(names), notifier(std::move(notifier)) {
}

namespace {
struct Available : MonitoredPredicate {
private:
  OVERRIDE(bool operator()(const MonitoredEvent& event) const) {
    return event.appeared();
  }
};
}  // namespace

bool ModelStateProbe::shoudNotify(MonitoredStore& store) const {
  for (auto& name : names) {
    if (!store.ready(name, Available()))
      return false;
  }
  return true;
}

bool ModelStateProbe::tryNotify(MonitoredStore& store) const {
  return shoudNotify(store) ? (notifier(), true) : false;
}

}  // namespace serving
}  // namespace adlik
