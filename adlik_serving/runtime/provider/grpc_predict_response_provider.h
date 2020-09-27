// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_PROVIDER_GRPC_PREDICT_RESPONSE_PROVIDER_H
#define ADLIK_SERVING_RUNTIME_PROVIDER_GRPC_PREDICT_RESPONSE_PROVIDER_H

#include <string>
#include <vector>

#include "adlik_serving/runtime/provider/predict_response_provider.h"

namespace adlik {
namespace serving {

struct PredictResponse;
struct PredictRequest;

struct GRPCPredictResponseProvider : PredictResponseProvider {
  GRPCPredictResponseProvider(const PredictRequest&, PredictResponse& response);

  virtual void* addOutput(const std::string& name,
                          tensorflow::DataType dtype,
                          const DimsList& dims,
                          size_t buffer_byte_size) override;

  virtual std::string* addOutput(const std::string& name, tensorflow::DataType dtype, const DimsList& dims) override;

private:
  PredictResponse& rsp;
  const size_t batch_size;
  std::vector<std::string> output_names;
};

}  // namespace serving
}  // namespace adlik

#endif /* RUNTIME_PROVIDER_MODEL_GRPC_PREDICT_RESPONSE_PROVIDER_H_ */
