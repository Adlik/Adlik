// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HE4857464_7416_4F76_8B8F_1612ADF91328
#define HE4857464_7416_4F76_8B8F_1612ADF91328

#include "cub/base/uncloneable.h"
#include "cub/env/concurrent/notification.h"
#include "cub/env/concurrent/thread.h"

namespace cub {

struct LoopThread {
  LoopThread(thread_t f, int64_t interval);
  ~LoopThread();

  DISALLOW_COPY_AND_ASSIGN(LoopThread)
  DISALLOW_MOVE_AND_ASSIGN(LoopThread)

private:
  void loop(thread_t f, int64_t interval);
  void stop();

private:
  Thread* thread;
  Notification stoped;
};

}  // namespace cub

#endif
