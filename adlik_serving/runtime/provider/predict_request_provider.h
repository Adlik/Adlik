// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_PROVIDER_PREDICT_REQUEST_PROVIDER_H
#define ADLIK_SERVING_RUNTIME_PROVIDER_PREDICT_REQUEST_PROVIDER_H

#include <functional>
#include <string>
#include <vector>

#include "adlik_serving/framework/domain/model_id.h"
#include "tensorflow/core/framework/tensor.pb.h"

namespace adlik {
namespace serving {

using InputVisitor = std::function<bool(const std::string& name, const tensorflow::TensorProto& tensor)>;

struct PredictRequestProvider {
public:
  explicit PredictRequestProvider(const adlik::serving::ModelId& id) : id(id) {
  }

  // Return the requested model name.
  const std::string& modelName() const {
    return id.getName();
  }

  // Return the requested model version, or -1 if no specific version
  // was requested.
  int modelVersion() const {
    return id.getVersion();
  }

  virtual size_t batchSize() const = 0;
  virtual size_t inputSize() const = 0;
  virtual void visitInputs(InputVisitor) const = 0;

  using OutputNames = std::vector<std::string>;
  virtual void outputNames(OutputNames&) const = 0;

private:
  const adlik::serving::ModelId& id;
};

}  // namespace serving
}  // namespace adlik

#endif
