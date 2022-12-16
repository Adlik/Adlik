// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TOPSINFERENCE_MODEL_TOPSINFERENCE_UTIL_H
#define ADLIK_SERVING_RUNTIME_TOPSINFERENCE_MODEL_TOPSINFERENCE_UTIL_H

#include <vector>

#include <TopsInference/TopsInferRuntime.h>
#include "adlik_serving/framework/domain/dims_list.h"
#include "adlik_serving/framework/domain/model_config.pb.h"
#include "tensorflow/core/lib/core/status.h"

namespace topsinference_runtime {
bool ConvertDims(const std::vector<size_t>&, adlik::serving::DimsList&);

}  // namespace topsinference_runtime
#endif
