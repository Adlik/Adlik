#include "cub/env/posix/posix_time.h"

#include <algorithm>
#include <limits>

#include <errno.h>
#include <sys/time.h>
#include <time.h>

namespace cub {

namespace {
struct TimeSpec : timespec {
  TimeSpec() {
    tv_sec = 0;
    tv_nsec = 0;
  }

  uint64_t now(uint64_t seconds) {
    ::clock_gettime(CLOCK_REALTIME, this);
    return static_cast<uint64_t>(tv_sec) * seconds +
           static_cast<uint64_t>(tv_nsec);
  }

  void sleep(uint64_t& micros) {
    elapse(micros);
    while (nanosleep(this, this) && errno == EINTR)
      ;
  }

private:
  void elapse(uint64_t& micros) {
    elapseSeconds(micros);
    elapseNanos(micros);
  }

  void elapseSeconds(uint64_t& micros) {
    if (micros >= 1e6) {
      tv_sec =
          std::min<uint64_t>(micros / 1e6, std::numeric_limits<time_t>::max());
      micros -= static_cast<uint64_t>(tv_sec) * 1e6;
    }
  }

  void elapseNanos(uint64_t& micros) {
    if (micros < 1e6) {
      tv_nsec = 1000 * micros;
      micros = 0;
    }
  }
};
}  // namespace

uint64_t PosixTime::nowNanos() const {
  TimeSpec ts;
  return ts.now(SECONDS);
}

void PosixTime::sleep(uint64_t micros) const {
  while (micros > 0) {
    TimeSpec ts;
    ts.sleep(micros);
  }
}

}  // namespace cub
