#include "cub/log/log.h"
#include <stdlib.h>
#include "cub/env/time/time.h"
#include "cub/string/str_utils.h"

namespace cub {

LogStream::LogStream(const char* file, int line, LogLevel level)
  : file(file), line(line), level(level) {
}

namespace {
inline int toInt(const char* str) {
  return str != nullptr ? strutils::to_int32(str) : 0;
}

int threshold() {
  static auto value = toInt(getenv("SL_CXX_MIN_LOG_LEVEL"));
  return value;
}
}  // namespace

inline void LogStream::trace() const {
  char buf[32];
  timesystem().format(buf);
  fprintf(
      stderr,
      "%s: %c %s:%d] %s\n",
      buf,
      "IWEF"[level],
      file,
      line,
      str().c_str());
}

LogStream::~LogStream() {
  if (level >= threshold()) {
    trace();
  }
}

}  // namespace cub
