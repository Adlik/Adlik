#include "cub/env/posix/posix_concurrent.h"
#include <thread>
#include "cub/env/time/time.h"

namespace cub {

namespace {
struct JoinableThread : Thread {
  JoinableThread(thread_t f) : thread(f) {
  }

  ~JoinableThread() override {
    thread.join();
  }

private:
  std::thread thread;
};
}  // namespace

Thread* PosixConcurrent::start(thread_t f) {
  return new JoinableThread(f);
}

void PosixConcurrent::sched(thread_t f) {
  std::thread thread(f);
  thread.detach();
}

void PosixConcurrent::schedAfter(int64_t micros, thread_t f) {
  sched([this, micros, f]() {
    ROLE(Time).sleep(micros);
    f();
  });
}

}  // namespace cub
