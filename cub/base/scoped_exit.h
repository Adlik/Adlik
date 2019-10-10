// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HAC57EDDB_4C76_4370_876A_05C7A7318E09
#define HAC57EDDB_4C76_4370_876A_05C7A7318E09

#include "cub/base/symbol.h"

namespace cub {

template <typename F>
struct ScopedExit {
  ScopedExit(F f) : f(f) {
  }

  ~ScopedExit() {
    f();
  }

private:
  F f;
};

template <typename F>
ScopedExit<F> make_scoped_exit(F f) {
  return {f};
};

#define SCOPED_EXIT(code) auto UNIQUE_NAME(scoped_exit_) = cub::make_scoped_exit((code))

}  // namespace cub

#endif
