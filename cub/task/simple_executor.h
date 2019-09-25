#ifndef HC34960A8_7172_49C5_A0B6_17143F29D901
#define HC34960A8_7172_49C5_A0B6_17143F29D901

#include "cub/task/executor.h"

namespace cub {

struct SimpleExecutor : Executor {
private:
  void schedule(thread_t) override;
};

}  // namespace cub

#endif
