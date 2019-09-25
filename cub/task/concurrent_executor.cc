#include "cub/task/concurrent_executor.h"

namespace cub {

ConcurrentExecutor::ConcurrentExecutor(int numThreads) : pool(numThreads) {
}

void ConcurrentExecutor::schedule(thread_t f) {
  pool.schedule(std::move(f));
}

}  // namespace cub
