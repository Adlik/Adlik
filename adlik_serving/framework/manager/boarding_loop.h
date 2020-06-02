// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HE3A2D336_C30C_46A5_8B53_973228C2FB87
#define HE3A2D336_C30C_46A5_8B53_973228C2FB87

#include <memory>
#include <vector>

#include "adlik_serving/framework/domain/model_stream.h"
#include "adlik_serving/framework/domain/model_target.h"
#include "cub/env/concurrent/loop_thread.h"
#include "cub/env/concurrent/mutex.h"

namespace adlik {
namespace serving {

struct ManagedStore;

struct BoardingLoop : ModelTarget {
  BoardingLoop();

private:
  OVERRIDE(void update(ModelStream&));

public:
  void poll();

private:
  std::unique_ptr<cub::LoopThread> loop;

  cub::Mutex mu;
  std::vector<ModelStream> streams;

private:
  USE_ROLE(ManagedStore);
};

}  // namespace serving
}  // namespace adlik

#endif
