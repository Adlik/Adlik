// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HA0B5DDCE_E254_4397_B389_ADEBC2EED253
#define HA0B5DDCE_E254_4397_B389_ADEBC2EED253

#include <memory>

#include "adlik_serving/framework/domain/model_handle.h"

namespace adlik {
namespace serving {

struct SharedModelHandle : ModelHandle {
  SharedModelHandle(std::shared_ptr<ModelHandle>);

private:
  OVERRIDE(cub::AnyPtr model());

private:
  std::shared_ptr<ModelHandle> handle;

private:
  IMPL_ROLE_WITH_OBJ(ModelId, handle->ROLE(ModelId))
};

}  // namespace serving
}  // namespace adlik

#endif
