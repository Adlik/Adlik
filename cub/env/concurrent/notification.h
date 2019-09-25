#ifndef HE09984E6_309D_4E7D_AEE7_E43A1B3E9FA6
#define HE09984E6_309D_4E7D_AEE7_E43A1B3E9FA6

#include <atomic>
#include "cub/env/concurrent/condition_variable.h"

namespace cub {

struct Notification {
  Notification();
  ~Notification();

  void wait();
  bool wait(int64_t micros);
  void notify();
  bool wasNotified() const;

private:
  Mutex mu;
  ConditionVariable cv;
  std::atomic<bool> notified;
};

}  // namespace cub

#endif
