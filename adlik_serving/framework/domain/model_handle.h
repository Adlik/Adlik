// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H01CE3A99_04F4_4BEC_92CE_0293C2B01E6A
#define H01CE3A99_04F4_4BEC_92CE_0293C2B01E6A

#include <string>

#include "cub/dci/role.h"
#include "cub/mem/any_ptr.h"

namespace adlik {
namespace serving {

struct ModelId;

DEFINE_ROLE(ModelHandle) {
  HAS_ROLE(ModelId);
  ABSTRACT(cub::AnyPtr model());
};

}  // namespace serving
}  // namespace adlik

#endif
