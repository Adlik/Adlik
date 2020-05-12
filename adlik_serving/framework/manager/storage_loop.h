// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HC283507B_8DAB_41AD_B40F_E0010001A0B0
#define HC283507B_8DAB_41AD_B40F_E0010001A0B0

#include <memory>

#include "adlik_serving/framework/domain/model_source.h"
#include "cub/env/concurrent/loop_thread.h"

namespace adlik {
namespace serving {

struct ModelStore;
struct ModelOptions;
struct ModelTarget;

struct StorageLoop : ModelSource {
  void poll();
  void once();
private:
  int64_t interval() const;
  void update(ModelTarget*);

private:
  OVERRIDE(void connect(ModelTarget&));

private:
  std::unique_ptr<cub::LoopThread> loop;
  ModelTarget* modelTarget;

private:
  USE_ROLE(ModelOptions);
  USE_ROLE(ModelStore);
};

}  // namespace serving
}  // namespace adlik

#endif
