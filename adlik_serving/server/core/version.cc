// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/core/version.h"

#include "cub/base/symbol.h"

namespace adlik {
namespace serving {

#define SL_MAJOR_VERSION 1
#define SL_MINOR_VERSION 0
#define SL_PATCH_VERSION 0

// SL_VERSION_SUFFIX is non-empty for pre-releases.
// (e.g. "alpha", "alpha.1", "beta", "rc", "rc.1", "dev")
#define SL_VERSION_SUFFIX "dev"

#define SL_VERSION_STRING     \
  STRINGIZE(SL_MAJOR_VERSION) \
  "." STRINGIZE(SL_MINOR_VERSION) "." STRINGIZE(SL_PATCH_VERSION) "-" SL_VERSION_SUFFIX

const char* version() {
  return SL_VERSION_STRING;
}

}  // namespace serving
}  // namespace adlik
