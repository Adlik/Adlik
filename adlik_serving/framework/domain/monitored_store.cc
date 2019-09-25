#include "adlik_serving/framework/domain/monitored_store.h"

namespace adlik {
namespace serving {

void MonitoredStore::update(const MonitoredEvent& event) {
  models[event.name()].update(event);
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
