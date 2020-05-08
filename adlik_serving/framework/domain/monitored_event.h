// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H4A7675F9_49E0_4581_8BD3_7F97D99F6731
#define H4A7675F9_49E0_4581_8BD3_7F97D99F6731

#include <stdint.h>

#include "adlik_serving/framework/domain/model_state.h"

namespace adlik {
namespace serving {

struct MonitoredEvent {
  MonitoredEvent();
  MonitoredEvent(const ModelState& state, uint64_t micros);

  bool appeared() const;
  const std::string& name() const;
  int version() const;
  std::string str() const;

private:
  ModelState state;
  uint64_t micros;
};

}  // namespace serving
}  // namespace adlik

#endif
