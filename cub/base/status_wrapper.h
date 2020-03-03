#ifndef CUB_BASE_STATUS_WRAPPER_H
#define CUB_BASE_STATUS_WRAPPER_H

#include "cub/base/status.h"

#include <memory>
#include <string>

namespace cub {

struct StatusWrapper {
  StatusWrapper() {
  }

  StatusWrapper(Status code, const std::string& msg);
  StatusWrapper(const StatusWrapper& s);

  void operator=(const StatusWrapper& s);

  static StatusWrapper OK() {
    return StatusWrapper();
  }

  bool ok() const {
    return (state == nullptr);
  }

  Status code() const {
    return ok() ? Success : state->code;
  }

  const std::string& error_message() const {
    return ok() ? empty_string() : state->msg;
  }

  bool operator==(const StatusWrapper& x) const;
  bool operator!=(const StatusWrapper& x) const;

  void update(const StatusWrapper& new_status);

  std::string toString() const;

  void ignoreError() const;

private:
  static const std::string& empty_string();

  struct State {
    Status code;
    std::string msg;
  };
  std::unique_ptr<State> state;

  void slowCopyFrom(const State* src);
};

inline StatusWrapper::StatusWrapper(const StatusWrapper& s)
    : state((s.state == nullptr) ? nullptr : std::make_unique<State>(*s.state)) {
}

inline void StatusWrapper::operator=(const StatusWrapper& s) {
  if (state != s.state) {
    slowCopyFrom(s.state.get());
  }
}

inline bool StatusWrapper::operator==(const StatusWrapper& x) const {
  return (this->state == x.state) || (toString() == x.toString());
}

inline bool StatusWrapper::operator!=(const StatusWrapper& x) const {
  return !(*this == x);
}

}  // namespace cub

#endif
