#include "cub/env/concurrent/mutex.h"
#include "nsync_mu.h"

namespace cub {

namespace {
inline nsync::nsync_mu* mutex(void* native) {
  return static_cast<nsync::nsync_mu*>(native);
}
}  // namespace

void* Mutex::native() {
  static_assert(sizeof(nsync::nsync_mu) <= sizeof(Impl), "");
  return reinterpret_cast<nsync::nsync_mu*>(&mu);
}

Mutex::Mutex() {
  nsync::nsync_mu_init(mutex(native()));
}

void Mutex::lock() {
  nsync::nsync_mu_lock(mutex(native()));
}

bool Mutex::tryLock() {
  return nsync::nsync_mu_trylock(mutex(native())) != 0;
}

void Mutex::unlock() {
  nsync::nsync_mu_unlock(mutex(native()));
}

void Mutex::lockShared() {
  nsync::nsync_mu_rlock(mutex(native()));
}

bool Mutex::tryLockShared() {
  return nsync::nsync_mu_rtrylock(mutex(native())) != 0;
}

void Mutex::unlockShared() {
  nsync::nsync_mu_runlock(mutex(native()));
}

}  // namespace cub
