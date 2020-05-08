// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H4D8CF9F1_DCDC_4902_9074_031C3CECD414
#define H4D8CF9F1_DCDC_4902_9074_031C3CECD414

#include <stdint.h>
#include <time.h>

#include "cub/env/env.h"

namespace cub {

DEFINE_ROLE(Time) {
  enum : uint64_t {
    NANOS = 1,
    MICROS = 1000 * NANOS,
    MILLIS = 1000 * MICROS,
    SECONDS = 1000 * MILLIS,
  };

  time_t nowTime() const;
  uint64_t nowSeconds() const;
  uint64_t nowMicros() const;

  template <size_t N>
  void format(char(&buf)[N]) const {
    auto now = nowTime();
    tm local_time;
    localtime_r(&now, &local_time);
    strftime(buf, N, "%Y-%m-%d %H:%M:%S", &local_time);
  }

  ABSTRACT(uint64_t nowNanos() const);
  ABSTRACT(void sleep(uint64_t micros) const);
};

inline Time& timesystem() {
  return Env::inst().ROLE(Time);
}

}  // namespace cub

#endif
