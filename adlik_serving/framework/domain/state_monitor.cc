// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/state_monitor.h"

#include "adlik_serving/framework/domain/event_bus.h"
#include "adlik_serving/framework/domain/model_store.h"
#include "adlik_serving/framework/domain/monitored_event.h"
#include "cub/env/concurrent/auto_lock.h"
#include "cub/env/concurrent/notification.h"
#include "cub/log/log.h"

namespace adlik {
namespace serving {

void StateMonitor::connect(EventBus& bus) {
  bus.subscribe(this);
}

template <typename Iterator>
inline void StateMonitor::tryNotify(Iterator& i) {
  if (i->tryNotify(store)) {
    i = probes.erase(i);
  } else {
    i++;
  }
}

inline void StateMonitor::tryNotify() {
  for (auto i = probes.begin(); i != probes.end();) {
    tryNotify(i);
  }
}

struct StateMonitor::LoadingWaiter : ModelConfigVisitor {
  LoadingWaiter(StateMonitor& self) : self(self) {
  }

  void wait() {
    cub::Notification done;
    enqueue([&done]() { done.notify(); });
    done.wait();
  }

private:
  void enqueue(MonitoredNotifier done) {
    cub::AutoLock lock(self.mu);
    self.probes.emplace_back(names, std::move(done));
    self.tryNotify();
  }

private:
  OVERRIDE(void visit(const ModelConfig& model)) {
    names.push_back(model.getModelName());
  }

private:
  StateMonitor& self;
  std::vector<std::string> names;
};

void StateMonitor::wait() {
  LoadingWaiter waiter(*this);
  ROLE(ModelStore).models(waiter);
  waiter.wait();
}

void StateMonitor::handle(const MonitoredEvent& event) {
  cub::AutoLock lock(mu);
  INFO_LOG << "model state monitor receive event: " << event.str();
  store.update(event);
  tryNotify();
}

}  // namespace serving
}  // namespace adlik
