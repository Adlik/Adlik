// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef CUB_CSV_TEST_UTIL_H
#define CUB_CSV_TEST_UTIL_H

#include <string>

#include "cub/env/fs/path.h"

namespace cub {

inline std::string TEST_FILE(const std::string& csv_path) {
  return paths("cub/csv/test_data", csv_path);
}

}  // namespace cub

#endif
