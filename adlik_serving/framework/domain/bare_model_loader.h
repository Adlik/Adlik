// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HC3AD9929_FCA5_459A_BA2B_B39E89148F2C
#define HC3AD9929_FCA5_459A_BA2B_B39E89148F2C

#include "cub/base/status.h"
#include "cub/dci/role.h"

namespace adlik {
namespace serving {

DEFINE_ROLE(BareModelLoader) {
  ABSTRACT(cub::Status loadBare());
  ABSTRACT(cub::Status unloadBare());
};

}  // namespace serving
}  // namespace adlik

#endif
