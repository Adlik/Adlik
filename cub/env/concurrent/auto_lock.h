#ifndef H91A7D2E3_7EDF_4994_811B_5168E331A336
#define H91A7D2E3_7EDF_4994_811B_5168E331A336

#include <mutex>
#include "cub/env/concurrent/mutex.h"

namespace cub {

struct AutoLock {
  explicit AutoLock(Mutex& mu) : mu(&mu) {
    mu.lock();
  }

  AutoLock(Mutex& mu, std::try_to_lock_t) : mu(&mu) {
    if (!mu.tryLock()) {
      this->mu = nullptr;
    }
  }

  AutoLock(AutoLock&& lock) noexcept : mu(lock.mu) {
    lock.mu = nullptr;
  }

  ~AutoLock() {
    if (mu != nullptr) {
      mu->unlock();
    }
  }

  operator bool() const {
    return mu != nullptr;
  }

  void* native() const {
    return mu->native();
  }

private:
  Mutex* mu;
};

}  // namespace cub

#endif
