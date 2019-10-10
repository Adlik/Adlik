// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HE9A6150D_2295_44C4_9D57_3FA8C51688C4
#define HE9A6150D_2295_44C4_9D57_3FA8C51688C4

#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct ModelStream;

DEFINE_ROLE(ModelTarget) {
  ABSTRACT(void update(ModelStream&));
};

}  // namespace serving
}  // namespace adlik

#endif
