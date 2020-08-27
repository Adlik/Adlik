// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_PROVIDER_PREDICT_REQUEST_PROVIDER_H
#define ADLIK_SERVING_RUNTIME_PROVIDER_PREDICT_REQUEST_PROVIDER_H

#include <functional>
#include <string>
#include <vector>

#include "tensorflow/core/framework/tensor.pb.h"

namespace adlik {
namespace serving {

using InputVisitor = std::function<bool(const std::string& name, const tensorflow::TensorProto& tensor)>;

struct PredictRequestProvider {
  virtual ~PredictRequestProvider() = default;

  virtual size_t batchSize() const = 0;
  virtual void visitInputs(InputVisitor) const = 0;
};

}  // namespace serving
}  // namespace adlik

#endif
