#include "cub/env/concurrent/notification.h"
#include "cub/env/concurrent/auto_lock.h"

namespace cub {

Notification::Notification() : notified(0) {
}

Notification::~Notification() {
  cub::AutoLock lock(mu);
}

bool Notification::wasNotified() const {
  return notified.load(std::memory_order_acquire);
}

void Notification::wait() {
  if (!wasNotified()) {
    cub::AutoLock lock(mu);
    while (!wasNotified()) {
      cv.wait(lock);
    }
  }
}

bool Notification::wait(int64_t micros) {
  auto notified = wasNotified();
  if (!notified) {
    cub::AutoLock lock(mu);
    do {
      notified = wasNotified();
    } while (!notified && !cv.wait(lock, micros));
  }
  return notified;
}

void Notification::notify() {
  cub::AutoLock lock(mu);
  notified.store(true, std::memory_order_release);
  cv.notifyAll();
}

}  // namespace cub
