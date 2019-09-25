#include "adlik_serving/framework/manager/time_stats.h"

#include "cub/log/log.h"

namespace adlik {
namespace serving {

using namespace std::chrono;

TimeStats::TimeStats(const std::string& message) : message(message), start(std::chrono::high_resolution_clock::now()) {
}
TimeStats::~TimeStats() {
  end = std::chrono::high_resolution_clock::now();
  float totalTime = duration<float, std::milli>(end - start).count();
  INFO_LOG << message << ", time (milliseconds): " << totalTime;
}
}  // namespace serving
}  // namespace adlik
