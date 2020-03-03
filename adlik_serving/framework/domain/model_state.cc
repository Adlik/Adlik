// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/model_state.h"

#include "adlik_serving/framework/domain/bare_model_loader.h"
#include "cub/base/assertions.h"

namespace adlik {
namespace serving {

ModelState::ModelState(const ModelId& id) : ModelId(id) {
}

const char* ModelState::str() const {
  switch (state) {
    case NEW:
      return "NEW";
    case LOADING:
      return "LOADING";
    case READY:
      return "READY";
    case UNLOADING:
      return "UNLOADING";
    case DISABLED:
      return "DISABLED";
    case ERROR:
      return "ERROR";
    default:
      return "UNKNOWN";
  }
}

inline cub::Status ModelState::transfer(State from, State to) {
  return state != from ? error(cub::Internal) : ok(to);
}

inline cub::Status ModelState::transfer(State from, State to, cub::Status status) {
  return cub::isSuccStatus(status) ? transfer(from, to) : error(status);
}

cub::Status ModelState::ok(State to) {
  state = to;
  return cub::Success;
}

cub::Status ModelState::error(cub::Status status) {
  state = ERROR;
  return status;
}

bool ModelState::untouched() const {
  return state == NEW;
}

bool ModelState::appeared() const {
  return ready() || finished();
}

bool ModelState::ready() const {
  return state == READY;
}

bool ModelState::healthy() const {
  return state != ERROR;
}

bool ModelState::finished() const {
  return state == ERROR || state == DISABLED;
}

bool ModelState::idle() const {
  return untouched() || finished();
}

bool ModelState::shouldStop() const {
  return idle() && !wasAspired();
}

cub::Status ModelState::loadBy(BareModelLoader& loader) {
  CUB_ASSERT_SUCC_CALL(transfer(NEW, LOADING));
  auto status = loader.loadBare();
  CUB_ASSERT_SUCC_CALL(transfer(LOADING, READY, status));
  return cub::Success;
}

cub::Status ModelState::unloadBy(BareModelLoader& loader) {
  CUB_ASSERT_SUCC_CALL(transfer(READY, UNLOADING));
  auto status = loader.unloadBare();
  CUB_ASSERT_SUCC_CALL(transfer(UNLOADING, DISABLED, status));
  return cub::Success;
}
}  // namespace serving
}  // namespace adlik
