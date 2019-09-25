#ifndef ADLIK_SERVING_FRAMEWORK_MANAGER_RUN_OPTIONS_H
#define ADLIK_SERVING_FRAMEWORK_MANAGER_RUN_OPTIONS_H

#include <cstdint>

namespace adlik {
namespace serving {

struct RunOptions {
  enum TraceLevel { NO_TRACE = 0, SOFTWARE_TRACE = 1, HARDWARE_TRACE = 2, FULL_TRACE = 3 };
  TraceLevel trace_level = NO_TRACE;

  // Time to wait for operation to complete in milliseconds.
  int64_t timeout_in_ms = 0;
};

}  // namespace serving
}  // namespace adlik

#endif
