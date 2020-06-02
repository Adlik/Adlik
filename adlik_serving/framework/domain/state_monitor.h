// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H2D5C8307_DC65_4726_A755_CCA25FBCCE22
#define H2D5C8307_DC65_4726_A755_CCA25FBCCE22

#include <deque>

#include "adlik_serving/framework/domain/event_handler.h"
#include "adlik_serving/framework/domain/monitored_probe.h"
#include "adlik_serving/framework/domain/monitored_store.h"
#include "cub/env/concurrent/mutex.h"

namespace adlik {
namespace serving {

struct EventBus;
struct ModelStore;

struct StateMonitor : private EventHandler {
  void connect(EventBus&);
  void wait();

private:
  OVERRIDE(void handle(const MonitoredEvent&));

private:
  struct LoadingWaiter;

  template <typename Iterator>
  void tryNotify(Iterator&);

  void tryNotify();

private:
  cub::Mutex mu;
  MonitoredStore store;
  std::deque<ModelStateProbe> probes;

private:
  USE_ROLE(ModelStore);
};

}  // namespace serving
}  // namespace adlik

#endif
