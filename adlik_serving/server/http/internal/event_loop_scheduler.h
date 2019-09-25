#ifndef ADLIK_SERVING_SERVER_HTTP_INTERNAL_EVENT_LOOP_SCHEDULER_H
#define ADLIK_SERVING_SERVER_HTTP_INTERNAL_EVENT_LOOP_SCHEDULER_H

#include <functional>

namespace adlik {
namespace serving {

struct EventLoopScheduler {
  virtual ~EventLoopScheduler() = default;

  virtual bool schedule(std::function<void()> fn) = 0;
};

}  // namespace serving
}  // namespace adlik

#endif
