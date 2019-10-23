// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_FRAMEWORK_DOMAIN_MODEL_CONFIG_HELPER_H
#define ADLIK_SERVING_FRAMEWORK_DOMAIN_MODEL_CONFIG_HELPER_H

#include <string>

#include "adlik_serving/framework/domain/dims_list.h"
#include "adlik_serving/framework/domain/model_config.pb.h"
#include "tensorflow/core/lib/core/status.h"

namespace adlik {
namespace serving {

using DimsList = ::google::protobuf::RepeatedField<::google::protobuf::int64>;

size_t GetDataTypeByteSize(const tensorflow::DataType dtype);
uint64_t GetSize(const tensorflow::DataType& dtype, const DimsList& dims);
uint64_t GetSize(const ModelInput& mio);
uint64_t GetSize(const ModelOutput& mio);
uint64_t GetSize(const int max_batch_size, const tensorflow::DataType& dtype, const DimsList& dims);

tensorflow::Status NormalizeModelConfig(ModelConfigProto& config);

tensorflow::Status ValidateModelConfig(const ModelConfigProto& config);

}  // namespace serving
}  // namespace adlik

#endif
