// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/model_options.h"

namespace adlik {
namespace serving {

ModelOptions::ModelOptions() : intervalInSecond(10), loadingMethod("automatic") {
}

void ModelOptions::subscribe(cub::ProgramOptions& prog) {
  auto options = new cub::OptionSet{{
      cub::option("model_base_path", &root, "path to export models"),
      cub::option("loading_method",
                  &loadingMethod,
                  "loading_method　value can be automatic/manual, when loading_method==manual "
                  "you can operate model by api"),
      cub::option(
          "fs_poll_wait_seconds", &intervalInSecond, "interval in seconds between each poll for new model version"),
  }};
  prog.add(options);
}

int64_t ModelOptions::getIntervalMs() const {
  return intervalInSecond * 1000 * 1000;
}

const std::string& ModelOptions::getBasePath() const {
  return root;
}

const std::string& ModelOptions::getLoadingMethod() const {
  return loadingMethod;
}

}  // namespace serving
}  // namespace adlik
