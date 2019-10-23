// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef FRAMEWORK_MANAGER_TIME_STATS_H
#define FRAMEWORK_MANAGER_TIME_STATS_H

#include <chrono>
#include <string>

namespace adlik {
namespace serving {

struct TimeStats {
  TimeStats(const std::string& message);
  ~TimeStats();

private:
  std::string message;
  std::chrono::high_resolution_clock::time_point start;
  std::chrono::high_resolution_clock::time_point end;
};
}  // namespace serving
}  // namespace adlik

#endif
