// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H329C4EF4_23F6_43E7_A05C_0A489AEB9D47
#define H329C4EF4_23F6_43E7_A05C_0A489AEB9D47

#include <memory>
#include <vector>

#include "cub/env/concurrent/mutex.h"

namespace adlik {
namespace serving {

struct ModelState;
struct EventHandler;

struct EventBus {
  void subscribe(EventHandler*);
  void publish(const ModelState&);

private:
  std::vector<EventHandler*> handlers;  // not owned.
};

}  // namespace serving
}  // namespace adlik

#endif
