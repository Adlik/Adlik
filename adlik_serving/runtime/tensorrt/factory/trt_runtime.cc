// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorrt/model/trt_model.h"
#include "adlik_serving/runtime/util/unique_batcher_runtime_helper.h"

namespace tensorrt {

DEFINE_UNIQUE_BATCHER_RUNTIME(tensorrt_plan, TrtModel);

}  // namespace tensorrt
