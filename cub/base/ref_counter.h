#ifndef HC6783643_ACBC_4DC1_A5B1_F6A3FDBEA123
#define HC6783643_ACBC_4DC1_A5B1_F6A3FDBEA123

#include <atomic>
#include <memory>

#include "cub/base/uncloneable.h"

namespace cub {

struct RefCounter {
  RefCounter();

  void ref() const;
  bool unref() const;
  bool exactlyOne() const;

protected:
  virtual ~RefCounter() {
  }

private:
  mutable std::atomic_int_fast32_t count;

  DISALLOW_COPY_AND_ASSIGN(RefCounter)
};

}  // namespace cub

#endif
