// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_UTIL_UNIQUE_BATCHER_RUNTIME_HELPER_H
#define ADLIK_SERVING_RUNTIME_UTIL_UNIQUE_BATCHER_RUNTIME_HELPER_H

#include "adlik_serving/runtime/util/default_runtime_helper.h"
#include "adlik_serving/runtime/util/unique_batcher_model_wrapper.h"

namespace adlik {
namespace serving {

#define DEFINE_UNIQUE_BATCHER_RUNTIME(Name, ModelType) \
  DEFINE_RUNTIME_HELPER(Name, adlik::serving::UniqueBatcherModelWrapper, ModelType)

}  // namespace serving
}  // namespace adlik

#endif
