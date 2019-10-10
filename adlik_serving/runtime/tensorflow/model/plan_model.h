// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HA2E2FA93_2A72_4EFE_8604_D3EF3F3CE7B1
#define HA2E2FA93_2A72_4EFE_8604_D3EF3F3CE7B1

#include "cub/dci/role.h"

namespace tensorflow {

class MetaGraphDef;
class Session;

DEFINE_ROLE(PlanModel) {
  HAS_ROLE(MetaGraphDef);
  HAS_ROLE(Session);
};

}  // namespace tensorflow

#endif
