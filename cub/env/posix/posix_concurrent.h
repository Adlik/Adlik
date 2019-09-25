#ifndef H068AD6F7_60FA_47DB_BCE6_219BD86EEFB9
#define H068AD6F7_60FA_47DB_BCE6_219BD86EEFB9

#include "cub/env/concurrent/concurrent.h"

namespace cub {

struct Time;

struct PosixConcurrent : Concurrent {
private:
  OVERRIDE(Thread* start(thread_t f));
  OVERRIDE(void sched(thread_t f));
  OVERRIDE(void schedAfter(int64_t micros, thread_t f));

private:
  USE_ROLE(Time);
};

}  // namespace cub

#endif
