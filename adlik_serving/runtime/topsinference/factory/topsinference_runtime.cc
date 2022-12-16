// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/topsinference/model/topsinference_model.h"
#include "adlik_serving/runtime/util/unique_batcher_runtime_helper.h"

namespace topsinference_runtime {

DEFINE_UNIQUE_BATCHER_RUNTIME(topsinference, EnflameModel);

}