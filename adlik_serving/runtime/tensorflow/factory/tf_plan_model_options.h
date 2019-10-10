// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HF05F1809_2334_4E59_9E43_AFED87B7D375
#define HF05F1809_2334_4E59_9E43_AFED87B7D375

#include "cub/cli/program_options.h"
#include "cub/string/str_utils.h"
#include "tensorflow/core/protobuf/config.pb.h"

namespace tensorflow {

struct BatchingParametersProto;

struct TfPlanModelOptions {
  TfPlanModelOptions();
  void subscribe(cub::ProgramOptions&);

  void batch(BatchingParametersProto&) const;
  void config(ConfigProto&) const;

  bool shouldBatching() const;
  bool shouldWarmup() const;

  template <typename F>
  void tags(F f) const {
    for (auto&& tag : cub::strutils::split(modelTags, ",")) {
      f(tag);
    }
  }

private:
  bool enableBatching;
  bool enableWarmup;

  std::string batchingFile;
  std::string modelTags;

  int parallelism;
  float fraction;
};

}  // namespace tensorflow

#endif
