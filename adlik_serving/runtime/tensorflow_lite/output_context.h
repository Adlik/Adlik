// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_OUTPUT_CONTEXT_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_OUTPUT_CONTEXT_H

#include "absl/types/span.h"
#include "adlik_serving/framework/domain/dims_list.h"
#include "tensorflow/core/framework/types.pb.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/lite/interpreter.h"

namespace adlik {
namespace serving {
class OutputContext {
  std::string name;

  // Cache.

  DimsList dimsListCache;

  OutputContext(int tensorIndex, std::string name, tensorflow::DataType dataType);

public:
  const int tensorIndex;
  const tensorflow::DataType dataType;

  const std::string& getName() const;
  const DimsList& calculateDimsList(const TfLiteIntArray& dims) noexcept;

  void readBatch(const TfLiteTensor& tfLiteTensor, size_t firstElement, size_t numElements, std::string& target);

  static OutputContext fromTfLiteTensor(int tensorIndex, const TfLiteTensor& tfLiteTensor);
};
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_OUTPUT_CONTEXT_H
