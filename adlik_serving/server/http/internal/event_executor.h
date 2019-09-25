#ifndef ADLIK_SERVING_SERVER_HTTP_INTERNAL_EVENT_EXECUTOR_H
#define ADLIK_SERVING_SERVER_HTTP_INTERNAL_EVENT_EXECUTOR_H

#include <functional>
#include <memory>

namespace adlik {
namespace serving {

struct EventExecutor {
  virtual ~EventExecutor() = default;
  virtual void schedule(std::function<void()> fn) = 0;

  static std::unique_ptr<EventExecutor> create(int num_threads);
};

}  // namespace serving
}  // namespace adlik

#endif
