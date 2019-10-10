// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/env/time/time.h"

namespace cub {

uint64_t Time::nowMicros() const {
  return nowNanos() / MICROS;
}

uint64_t Time::nowSeconds() const {
  return nowNanos() / SECONDS;
}

time_t Time::nowTime() const {
  return static_cast<time_t>(nowSeconds());
}

}  // namespace cub
