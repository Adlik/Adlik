// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/monitored_model.h"

#include "adlik_serving/framework/domain/monitored_event.h"
#include "adlik_serving/framework/domain/monitored_predicate.h"

namespace adlik {
namespace serving {

void MonitoredModel::update(const MonitoredEvent& event) {
  versions[event.version()] = event;
}

bool MonitoredModel::ready(const MonitoredPredicate& pred) const {
  for (auto& p : versions) {
    if (pred(p.second))
      return true;
  }
  return false;
}

}  // namespace serving
}  // namespace adlik
