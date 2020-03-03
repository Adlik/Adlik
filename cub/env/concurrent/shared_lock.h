// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H3A67D0FD_46FA_4DF3_B088_970A9DA0633B
#define H3A67D0FD_46FA_4DF3_B088_970A9DA0633B

#include <mutex>

#include "cub/env/concurrent/mutex.h"

namespace cub {

struct SharedLock {
  explicit SharedLock(Mutex& mu) : mu(&mu) {
    mu.lockShared();
  }

  SharedLock(Mutex& mu, std::try_to_lock_t) : mu(&mu) {
    if (!mu.tryLockShared()) {
      this->mu = nullptr;
    }
  }

  SharedLock(SharedLock&& lock) noexcept : mu(lock.mu) {
    lock.mu = nullptr;
  }

  ~SharedLock() {
    if (mu != nullptr) {
      mu->unlockShared();
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
