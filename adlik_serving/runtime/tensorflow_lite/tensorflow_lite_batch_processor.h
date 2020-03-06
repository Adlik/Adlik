// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_BATCH_PROCESSOR_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_BATCH_PROCESSOR_H

#include "absl/hash/hash.h"
#include "adlik_serving/runtime/batching/batch_processor.h"
#include "adlik_serving/runtime/tensorflow_lite/tensor_shape_dims.h"
#include "tensorflow/core/framework/types.pb.h"
#include "tensorflow/lite/model.h"

namespace adlik {
namespace serving {
class TensorFlowLiteBatchProcessor : public BatchProcessor {
  using InputSignature = std::unordered_map<absl::string_view,
                                            std::tuple<tensorflow::DataType, TensorShapeDims>,
                                            absl::Hash<absl::string_view>>;

  const std::shared_ptr<tflite::FlatBufferModel> model;  // Make sure the model is alive when interpreter is alive.
  std::unique_ptr<tflite::Interpreter> interpreter;
  const InputSignature inputSignature;
  mutable InputSignature argumentSignatureCache;

  virtual tensorflow::Status processBatch(Batch<BatchingMessageTask>& batch) override;

public:
  TensorFlowLiteBatchProcessor(std::shared_ptr<tflite::FlatBufferModel> model,
                               std::unique_ptr<tflite::Interpreter> interpreter);
};
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSORFLOW_LITE_BATCH_PROCESSOR_H
