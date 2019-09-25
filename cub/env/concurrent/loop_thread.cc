#include "cub/env/concurrent/loop_thread.h"
#include "cub/env/concurrent/concurrent.h"
#include "cub/env/time/time.h"

namespace cub {

namespace {
inline Thread* start(thread_t f) {
  return concurrent().start(f);
}

inline int64_t nowMicros() {
  return timesystem().nowMicros();
}

inline void sleep(int64_t micros) {
  timesystem().sleep(micros);
}

inline int64_t endMicros(int64_t begin) {
  return std::max(nowMicros(), begin);
}
}  // namespace

void LoopThread::loop(thread_t f, int64_t interval) {
  while (!stoped.wasNotified()) {
    auto begin = nowMicros();
    f();
    auto end = endMicros(begin);
    auto deadline = begin + interval;
    if (deadline > end) {
      sleep(deadline - end);
    }
  }
}

LoopThread::LoopThread(thread_t f, int64_t interval) {
  thread = start([this, f, interval]() { loop(f, interval); });
}

inline void LoopThread::stop() {
  if (!stoped.wasNotified()) {
    stoped.notify();
  }
}

LoopThread::~LoopThread() {
  stop();
  delete thread;
}

}  // namespace cub
