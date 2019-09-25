#include "adlik_serving/framework/domain/monitored_event.h"

namespace adlik {
namespace serving {

MonitoredEvent::MonitoredEvent() : micros(0) {
}

MonitoredEvent::MonitoredEvent(const ModelState& state, uint64_t micros) : state(state), micros(micros) {
}

bool MonitoredEvent::appeared() const {
  return state.appeared();
}

const std::string& MonitoredEvent::name() const {
  return state.getName();
}

int MonitoredEvent::version() const {
  return state.getVersion();
}

std::string MonitoredEvent::str() const {
  return std::string() + name() + ":" + std::to_string(version()) + ":" + state.str();
}

}  // namespace serving
}  // namespace adlik
