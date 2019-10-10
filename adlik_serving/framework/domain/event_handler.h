// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H4CECD59D_D4C4_4E43_90D1_ECDED75A3B80
#define H4CECD59D_D4C4_4E43_90D1_ECDED75A3B80

#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct MonitoredEvent;

DEFINE_ROLE(EventHandler) {
  ABSTRACT(void handle(const MonitoredEvent&));
};

}  // namespace serving
}  // namespace adlik

#endif
