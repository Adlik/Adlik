// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORRT_MODEL_TRT_INSTANCE_H
#define ADLIK_SERVING_RUNTIME_TENSORRT_MODEL_TRT_INSTANCE_H

#include <memory>
#include <string>
#include <vector>

#include "adlik_serving/runtime/batching/batch_processor.h"
#include "cub/base/fwddecl.h"

namespace adlik {
namespace serving {
struct ModelConfigProto;
}
}  // namespace adlik

namespace tensorrt {

tensorflow::Status CreateTrtInstance(const adlik::serving::ModelConfigProto&,
                                     const std::string& name,
                                     const int gpu_device,
                                     const std::vector<char>& model_data,
                                     std::unique_ptr<adlik::serving::BatchProcessor>*);

}  // namespace tensorrt

#endif
