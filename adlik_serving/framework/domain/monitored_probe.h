// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H7EF71BD7_D6CF_492C_971D_43716285BE66
#define H7EF71BD7_D6CF_492C_971D_43716285BE66

#include <string>
#include <vector>

#include "adlik_serving/framework/domain/monitored_notifier.h"

namespace adlik {
namespace serving {

struct MonitoredStore;

struct ModelStateProbe {
  ModelStateProbe(const std::vector<std::string>& names, MonitoredNotifier);

  bool tryNotify(MonitoredStore&) const;

private:
  bool shoudNotify(MonitoredStore&) const;

private:
  std::vector<std::string> names;
  MonitoredNotifier notifier;
};

}  // namespace serving
}  // namespace adlik

#endif
