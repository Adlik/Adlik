// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/libtorch/model/libtorch_model.h"
#include "adlik_serving/runtime/util/unique_batcher_runtime_helper.h"

namespace libtorch_runtime {

DEFINE_UNIQUE_BATCHER_RUNTIME(libtorch, LibtorchModel);

}
