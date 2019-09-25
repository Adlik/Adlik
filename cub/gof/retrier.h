#ifndef HCE07E176_4626_4C80_A209_B74B52220F98
#define HCE07E176_4626_4C80_A209_B74B52220F98

#include <functional>
#include "cub/base/status.h"

namespace cub {

class Retrier {
  using RetriedFn = std::function<Status()>;
  using IsCancelled = std::function<bool()>;

public:
  Retrier(uint32_t maxNumTries, int64_t intervalMicros);
  Status operator()(RetriedFn fn, IsCancelled cancelled) const;

private:
  bool finished(Status status, bool cancelled) const;
  void sleep() const;

private:
  uint32_t numTries = 0;
  uint32_t maxNumTries;
  int64_t intervalMicros;
};

}  // namespace cub

#endif
