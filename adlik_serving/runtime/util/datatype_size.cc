// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/util/datatype_size.h"

#include <cstddef>

namespace adlik {
namespace serving {

::std::size_t GetDataTypeSize(const tensorflow::DataType dtype) {
  switch (dtype) {
    case tensorflow::DataType::DT_BOOL:
      return 1;
    case tensorflow::DataType::DT_INT8:
      return 1;
    case tensorflow::DataType::DT_UINT8:
      return 1;
    case tensorflow::DataType::DT_INT16:
      return 2;
    case tensorflow::DataType::DT_INT32:
      return 4;
    case tensorflow::DataType::DT_INT64:
      return 8;
    case tensorflow::DataType::DT_HALF:
      return 2;
    case tensorflow::DataType::DT_FLOAT:
      return 4;
    case tensorflow::DataType::DT_DOUBLE:
      return 8;
    case tensorflow::DataType::DT_COMPLEX64:
      return 8;
    case tensorflow::DataType::DT_COMPLEX128:
      return 16;
    default:
      return 0;
  }
}

}  // namespace serving
}  // namespace adlik