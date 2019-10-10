// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H26909C00_E994_4F07_B3E5_C662D839B81D
#define H26909C00_E994_4F07_B3E5_C662D839B81D

#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct MonitoredEvent;

DEFINE_ROLE(MonitoredPredicate) {
  ABSTRACT(bool operator()(const MonitoredEvent&) const);
};

}  // namespace serving
}  // namespace adlik

#endif
