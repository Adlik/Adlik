#include "cub/gof/retrier.h"
#include "cub/env/env.h"
#include "cub/env/time/time.h"

namespace cub {

Retrier::Retrier(uint32_t maxNumTries, int64_t intervalMicros)
  : maxNumTries(maxNumTries), intervalMicros(intervalMicros) {
}

inline bool Retrier::finished(Status status, bool cancelled) const {
  return cancelled || status == Success || numTries == maxNumTries;
};

inline void Retrier::sleep() const {
  if (numTries > 0) {
    Env::inst().ROLE(Time).sleep(intervalMicros);
  }
}

Status Retrier::operator()(RetriedFn fn, IsCancelled cancelled) const {
  Status status = Failure;
  do {
    sleep();
    status = fn();
    numTries++;
  } while (!finished(status, cancelled()));
  return status;
}

}  // namespace cub
