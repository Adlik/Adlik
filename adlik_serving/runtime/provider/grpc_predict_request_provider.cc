// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/provider/grpc_predict_request_provider.h"

#include <set>

#include "adlik_serving/apis/predict.pb.h"
#include "cub/log/log.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

GRPCPredictRequestProvider::GRPCPredictRequestProvider(const PredictRequest& req) : req(req) {
}

tensorflow::Status GRPCPredictRequestProvider::create(const PredictRequest& request,
                                                      std::unique_ptr<GRPCPredictRequestProvider>* provider) {
  if (request.batch_size() < 1) {
    return tensorflow::errors::InvalidArgument("inference request batch-size must be >= 1 for models that ",
                                               "support batching, and must be 1 for models that don't ",
                                               "support batching");
  }

  for (auto& i : request.inputs()) {
    if (i.second.tensor_content().empty()) {
      return tensorflow::errors::InvalidArgument(
          "Input '",
          i.first,
          "' is not in bytes format, it should be serialized to bytes for tensorRT "
          "model");
    }
  }

  provider->reset(new GRPCPredictRequestProvider(request));
  return tensorflow::Status::OK();
}

size_t GRPCPredictRequestProvider::batchSize() const {
  return req.batch_size();
}

void GRPCPredictRequestProvider::visitInputs(InputVisitor visitor) const {
  for (auto& i : req.inputs()) {
    if (!visitor(i.first, i.second))
      break;
  }
}

}  // namespace serving
}  // namespace adlik
