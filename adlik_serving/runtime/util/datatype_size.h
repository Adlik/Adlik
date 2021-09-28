// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_UTIL_DATATYPE_SIZE_H
#define ADLIK_SERVING_RUNTIME_UTIL_DATATYPE_SIZE_H

#include "tensorflow/core/framework/types.pb.h"

namespace adlik {
namespace serving {

::std::size_t GetDataTypeSize(const tensorflow::DataType dtype);

}  // namespace serving
}  // namespace adlik
#endif