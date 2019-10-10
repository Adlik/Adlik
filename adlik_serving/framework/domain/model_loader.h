// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H4711C4A3_9604_4256_AA66_62CDD86A1456
#define H4711C4A3_9604_4256_AA66_62CDD86A1456

#include "cub/base/status.h"
#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct ModelState;
struct BareModelLoader;

DEFINE_ROLE(ModelLoader) {
  cub::Status load();
  cub::Status unload();

private:
  USE_ROLE(BareModelLoader);
  USE_ROLE(ModelState);
};

}  // namespace serving
}  // namespace adlik

#endif
