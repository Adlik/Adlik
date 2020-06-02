// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H4B4D8F72_A4E0_4120_A0F8_A817E87D3430
#define H4B4D8F72_A4E0_4120_A0F8_A817E87D3430

#include <unordered_map>

#include "adlik_serving/framework/domain/monitored_model.h"
#include "adlik_serving/framework/domain/monitored_predicate.h"
#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct MonitoredStore {
  void update(const MonitoredEvent&);
  bool ready(const std::string&, const MonitoredPredicate&) const;

private:
  std::unordered_map<std::string, MonitoredModel> models;
};

}  // namespace serving
}  // namespace adlik

#endif
