#ifndef HECE68A2F_8AC8_4621_B5E9_387341887A6A
#define HECE68A2F_8AC8_4621_B5E9_387341887A6A

#include <memory>
#include "cub/env/concurrent/thread.h"

namespace cub {

struct ThreadPool {
  ThreadPool(int numThreads);
  ~ThreadPool();

  void schedule(thread_t fn);
  void cancel();
  int getNumThreads() const;
  int getCurrentThreadId() const;

private:
  struct Impl;
  Impl* impl;
};

}  // namespace cub

#endif
