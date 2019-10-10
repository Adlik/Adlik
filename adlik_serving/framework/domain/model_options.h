// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H6A87ADCC_E678_4301_8D58_74E14B09AE17
#define H6A87ADCC_E678_4301_8D58_74E14B09AE17

#include "cub/cli/program_options.h"

namespace adlik {
namespace serving {

struct ModelOptions {
  ModelOptions();

  void subscribe(cub::ProgramOptions&);

  int64_t getIntervalMs() const;
  const std::string& getBasePath() const;

private:
  std::string root;
  int intervalInSecond;  // second
};

}  // namespace serving
}  // namespace adlik

#endif
