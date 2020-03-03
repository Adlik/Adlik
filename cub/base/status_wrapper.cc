// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/base/status_wrapper.h"

#include <assert.h>

namespace cub {

StatusWrapper::StatusWrapper(Status code, const std::string& msg) {
  assert(code != Success);
  state = std::make_unique<State>();
  state->code = code;
  state->msg = msg;
}

void StatusWrapper::update(const StatusWrapper& new_status) {
  if (ok()) {
    *this = new_status;
  }
}

void StatusWrapper::slowCopyFrom(const State* src) {
  if (src == nullptr) {
    state = nullptr;
  } else {
    state = std::make_unique<State>(*src);
  }
}

const std::string& StatusWrapper::empty_string() {
  static std::string empty;
  return empty;
}

}  // namespace cub
