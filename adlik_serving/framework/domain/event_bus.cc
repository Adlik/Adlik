// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/event_bus.h"

#include "adlik_serving/framework/domain/event_handler.h"
#include "adlik_serving/framework/domain/monitored_event.h"
#include "cub/env/time/time.h"

namespace adlik {
namespace serving {

void EventBus::subscribe(EventHandler* handle) {
  handlers.push_back(handle);
}

namespace {
inline uint64_t now() {
  return cub::timesystem().nowMicros();
}
}  // namespace

void EventBus::publish(const ModelState& state) {
  for (auto handler : handlers) {
    handler->handle({state, now()});
  }
}

}  // namespace serving
}  // namespace adlik
