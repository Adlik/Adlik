#include "cub/task/simple_executor.h"

namespace cub {

void SimpleExecutor::schedule(thread_t f) {
  f();
}

}  // namespace cub
