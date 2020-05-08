// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H38247538_297F_4A80_94D3_8A289788461B
#define H38247538_297F_4A80_94D3_8A289788461B

#include <sstream>

namespace cub {

struct LogStream : std::ostringstream {
  enum LogLevel {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
  };

  LogStream(const char* file, int line, LogLevel level);
  ~LogStream();

private:
  void trace() const;

private:
  const char* file;
  int line;
  int level;
};

#define LOG_STREAM(level) cub::LogStream(__FILE__, __LINE__, cub::LogStream::level)

#define DEBUG_LOG LOG_STREAM(LOG_DEBUG)
#define INFO_LOG LOG_STREAM(LOG_INFO)
#define WARN_LOG LOG_STREAM(LOG_WARN)
#define ERR_LOG LOG_STREAM(LOG_ERROR)
#define FATAL_LOG LOG_STREAM(LOG_FATAL)

}  // namespace cub

#endif
