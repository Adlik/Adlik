// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HF1206F94_31DB_4B9E_8C59_74E4406FFAC4
#define HF1206F94_31DB_4B9E_8C59_74E4406FFAC4

#include <map>

#include "adlik_serving/framework/domain/monitored_event.h"

namespace adlik {
namespace serving {

struct MonitoredPredicate;

struct MonitoredModel {
  void update(const MonitoredEvent&);
  bool ready(const MonitoredPredicate&) const;

private:
  std::map<int, MonitoredEvent, std::greater<int>> versions;
};

}  // namespace serving
}  // namespace adlik

#endif
