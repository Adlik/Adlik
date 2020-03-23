// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_OUTPUT_CONTEXT_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_OUTPUT_CONTEXT_H

#include "absl/types/span.h"
#include "adlik_serving/framework/domain/dims_list.h"
#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"

namespace adlik {
namespace serving {
class OutputContext {
  std::string name;

  // Cache.

  DimsList dimsListCache;

  OutputContext(int tensorIndex,
                std::string name,
                tensorflow::DataType dataType,
                tensor_tools::TfLiteTensorReader reader);

public:
  const int tensorIndex;
  const tensorflow::DataType dataType;
  tensor_tools::TfLiteTensorReader reader;

  const std::string& getName() const;
  const DimsList& calculateDimsList(const TfLiteIntArray& dims) noexcept;

  static OutputContext fromTfLiteTensor(int tensorIndex, const TfLiteTensor& tfLiteTensor);
};
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_OUTPUT_CONTEXT_H
