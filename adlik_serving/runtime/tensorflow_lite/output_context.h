// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_OUTPUT_CONTEXT_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_OUTPUT_CONTEXT_H

#include "adlik_serving/framework/domain/dims_list.h"
#include "tensorflow/core/framework/types.pb.h"
#include "tensorflow/lite/interpreter.h"

namespace adlik {
namespace serving {
class OutputContext {
  std::string name;
  DimsList dimsListCache;

  OutputContext(int tensorIndex, std::string name, tensorflow::DataType dataType);

public:
  const int tensorIndex;
  const tensorflow::DataType dataType;

  const std::string& getName() const;
  const DimsList& getDimsList(DimsList::value_type batchSize, const TfLiteIntArray& sourceDims);
  void reset() noexcept;

  static OutputContext fromTfLiteTensor(int tensorIndex, const TfLiteTensor& tfLiteTensor);
};
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_OUTPUT_CONTEXT_H
