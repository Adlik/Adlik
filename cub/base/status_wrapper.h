// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef CUB_BASE_STATUS_WRAPPER_H
#define CUB_BASE_STATUS_WRAPPER_H

#include <memory>
#include <string>

#include "cub/base/status.h"

namespace cub {

struct StatusWrapper {
  StatusWrapper();
  StatusWrapper(Status status, const std::string& msg);

  static StatusWrapper OK() {
    return StatusWrapper();
  }

  bool ok() const {
    return status == Success;
  }

  Status code() const {
    return status;
  }

  const std::string& error_message() const {
    return ok() ? empty_string : msg;
  }

private:
  static const std::string empty_string;

  Status status;
  std::string msg;
};

}  // namespace cub

#endif
