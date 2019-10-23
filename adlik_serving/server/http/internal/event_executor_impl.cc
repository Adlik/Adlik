#include "adlik_serving/server/http/internal/event_executor.h"

#include "cub/task/concurrent_executor.h"
#include <algorithm>

namespace adlik {
namespace serving {

namespace {
struct ExecutorImpl : EventExecutor {
  explicit ExecutorImpl(int num_threads) : executor(std::max(2, num_threads)) {
  }

  void schedule(std::function<void()> fn) override {
    executor.schedule(fn);
  }

private:
  cub::ConcurrentExecutor executor;
};
}  // namespace

std::unique_ptr<EventExecutor> EventExecutor::create(int num_threads) {
  return std::make_unique<ExecutorImpl>(num_threads);
}

}  // namespace serving
}  // namespace adlik
