#include "cub/base/ref_counter.h"

namespace cub {

RefCounter::RefCounter() : count(1) {
}

void RefCounter::ref() const {
  count.fetch_add(1, std::memory_order_relaxed);
}

bool RefCounter::unref() const {
  if (exactlyOne() || count.fetch_sub(1) == 1) {
    delete this;
    return true;
  } else {
    return false;
  }
}

bool RefCounter::exactlyOne() const {
  return count.load(std::memory_order_acquire) == 1;
}

}  // namespace cub
