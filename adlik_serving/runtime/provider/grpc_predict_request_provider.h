// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_PROVIDER_GRPC_PREDICT_REQUEST_PROVIDER_H
#define ADLIK_SERVING_RUNTIME_PROVIDER_GRPC_PREDICT_REQUEST_PROVIDER_H

#include <memory>

#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "tensorflow/core/lib/core/status.h"

namespace adlik {
namespace serving {

struct PredictRequest;

struct GRPCPredictRequestProvider : public PredictRequestProvider {
  static tensorflow::Status create(const PredictRequest&, std::unique_ptr<PredictRequestProvider>*);

  size_t batchSize() const override;
  void visitInputs(InputVisitor) const override;

  GRPCPredictRequestProvider(const PredictRequest& req);

private:
  const PredictRequest& req;
};

}  // namespace serving
}  // namespace adlik

#endif /* RUNTIME_PROVIDER_MODEL_GRPC_PREDICT_REQUEST_PROVIDER_H_ */
