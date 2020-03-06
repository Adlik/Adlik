// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow_lite/tensorflow_lite_model.h"
#include "adlik_serving/runtime/util/unique_batcher_runtime_helper.h"

namespace {
using adlik::serving::TensorFlowLiteModel;

DEFINE_UNIQUE_BATCHER_RUNTIME(TensorFlowLite, TensorFlowLiteModel);
}  // namespace
