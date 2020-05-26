// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/manager/boarding_loop.h"

#include "adlik_serving/framework/manager/boarding_functor.h"
#include "cub/env/concurrent/auto_lock.h"

namespace adlik {
namespace serving {

BoardingLoop::BoardingLoop() {
}

void BoardingLoop::poll() {
  auto action = [this] { this->once(); };
  loop.reset(new cub::LoopThread(action, 10 * 1000 /* ms */));
}

void BoardingLoop::once() {
  cub::AutoLock lock(this->mu);
  BoardingFunctor f(this->ROLE(ManagedStore));
  f(streams);
}

void BoardingLoop::update(ModelStream& stream) {
  cub::AutoLock lock(mu);
  streams.push_back(stream);
}

}  // namespace serving
}  // namespace adlik
