// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/model_options.h"

namespace adlik {
namespace serving {

ModelOptions::ModelOptions() : intervalInSecond(1) {
}

void ModelOptions::subscribe(cub::ProgramOptions& prog) {
  auto options = new cub::OptionSet{{
      cub::option("model_base_path", &root, "path to export models"),
      cub::option(
          "fs_poll_wait_seconds", &intervalInSecond, "interval in seconds between each poll for new model version"),
  }};
  return prog.add(options);
}

int64_t ModelOptions::getIntervalMs() const {
  return intervalInSecond * 1000 * 1000;
}

const std::string& ModelOptions::getBasePath() const {
  return root;
}

}  // namespace serving
}  // namespace adlik
