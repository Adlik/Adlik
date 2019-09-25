#ifndef H7E2C7B9A_AD64_4EE8_8988_1B77EADBEF87
#define H7E2C7B9A_AD64_4EE8_8988_1B77EADBEF87

#include "cub/env/time/time.h"

namespace cub {

struct PosixTime : Time {
private:
  OVERRIDE(uint64_t nowNanos() const);
  OVERRIDE(void sleep(uint64_t micros) const);
};

}  // namespace cub

#endif
