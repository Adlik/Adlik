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

struct GRPCPredictResponseProvider : PredictResponseProvider {
public:
  static tensorflow::Status create(const std::vector<std::string>& output_names,
                                   size_t batch_size,
                                   PredictResponse& rsp,
                                   std::unique_ptr<GRPCPredictResponseProvider>* provider);

  virtual void* addOutput(const std::string& name,
                          tensorflow::DataType dtype,
                          const DimsList& dims,
                          size_t buffer_byte_size) override;

  virtual std::string* addOutput(const std::string& name, tensorflow::DataType dtype, const DimsList& dims) override;

private:
  GRPCPredictResponseProvider(const std::vector<std::string>& output_names,
                              size_t batch_size,
                              PredictResponse& response);

  std::vector<std::string> output_names;
  PredictResponse& rsp;
  size_t batch_size;
};

}  // namespace serving
}  // namespace adlik

#endif /* RUNTIME_PROVIDER_MODEL_GRPC_PREDICT_RESPONSE_PROVIDER_H_ */
