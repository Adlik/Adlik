// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORRT_FACTORY_TRT_MODEL_OPTIONS_H
#define ADLIK_SERVING_RUNTIME_TENSORRT_FACTORY_TRT_MODEL_OPTIONS_H

#include "cub/base/fwddecl.h"

FWD_DECL_STRUCT(cub, ProgramOptions)

namespace tensorrt {

struct TrtModelOptions {
  TrtModelOptions();

  void subscribe(cub::ProgramOptions&);

private:
  bool enableBatching;
};

}  // namespace tensorrt

#endif
