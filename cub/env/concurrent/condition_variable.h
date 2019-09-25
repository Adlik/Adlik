#ifndef H3D5C68CE_1D25_490D_A2CB_D5EA3D79906A
#define H3D5C68CE_1D25_490D_A2CB_D5EA3D79906A

#include <condition_variable>
#include "cub/env/concurrent/auto_lock.h"

namespace cub {

struct ConditionVariable {
  ConditionVariable();

  // return true if timeout, otherwise return false.
  bool wait(AutoLock& lock, int64_t ms);
  void wait(AutoLock& lock);

  void notify();
  void notifyAll();

  void* native();

private:
  struct Impl {
    void* space[2];
  };

  bool wait(AutoLock&, const std::chrono::system_clock::time_point&);

private:
  Impl cv;
};

}  // namespace cub

#endif
