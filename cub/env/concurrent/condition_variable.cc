#include "cub/env/concurrent/condition_variable.h"
#include "nsync_cv.h"
#include "nsync_mu.h"

namespace cub {

void* ConditionVariable::native() {
  static_assert(sizeof(nsync::nsync_cv) <= sizeof(Impl), "");
  return reinterpret_cast<nsync::nsync_cv*>(&cv);
}

namespace {
inline nsync::nsync_cv* cond_var(void* native) {
  return static_cast<nsync::nsync_cv*>(native);
}
}  // namespace

ConditionVariable::ConditionVariable() {
  nsync::nsync_cv_init(cond_var(native()));
}

namespace {
inline nsync::nsync_mu* mutex(AutoLock& lock) {
  return static_cast<nsync::nsync_mu*>(lock.native());
}
}  // namespace

void ConditionVariable::wait(AutoLock& lock) {
  nsync::nsync_cv_wait(cond_var(native()), mutex(lock));
}

inline bool ConditionVariable::wait(
    AutoLock& lock,
    const std::chrono::system_clock::time_point& timeout) {
  return nsync::nsync_cv_wait_with_deadline(
             cond_var(native()), mutex(lock), timeout, nullptr) != 0;
}

bool ConditionVariable::wait(AutoLock& lock, int64_t ms) {
  return wait(
      lock, std::chrono::system_clock::now() + std::chrono::milliseconds(ms));
}

void ConditionVariable::notify() {
  nsync::nsync_cv_signal(cond_var(native()));
}

void ConditionVariable::notifyAll() {
  nsync::nsync_cv_broadcast(cond_var(native()));
}

}  // namespace cub
