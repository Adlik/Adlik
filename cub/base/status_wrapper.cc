// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/base/status_wrapper.h"

#include <assert.h>

namespace cub {

StatusWrapper::StatusWrapper() : status(Success) {
}

StatusWrapper::StatusWrapper(Status code, const std::string& msg) : status(code), msg(msg) {
}

const std::string StatusWrapper::empty_string;

}  // namespace cub
