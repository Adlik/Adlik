// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H13A77DD9_1B8F_4C6E_9109_D2A9B7AFD9FB
#define H13A77DD9_1B8F_4C6E_9109_D2A9B7AFD9FB

#include "cub/dci/role.h"

namespace cub {

struct Time;
struct Concurrent;
struct FileSystem;

DEFINE_ROLE(Env) {
  static Env& inst();

  HAS_ROLE(Time);
  HAS_ROLE(Concurrent);
  HAS_ROLE(FileSystem);
};

}  // namespace cub

#endif
