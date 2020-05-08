// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/provider/grpc_predict_request_provider.h"

#include <set>

#include "adlik_serving/apis/predict.pb.h"
#include "cub/log/log.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

GRPCPredictRequestProvider::GRPCPredictRequestProvider(const ModelId& id, const PredictRequest& req)
    : PredictRequestProvider(id), req(req) {
  // content_delivered_.resize(request_.raw_input_size(), false);
}

tensorflow::Status GRPCPredictRequestProvider::create(const ModelId& id,
                                                      const PredictRequest& request,
                                                      std::unique_ptr<GRPCPredictRequestProvider>* provider) {
  // Make sure the request has a batch-size > 0. Even for models that
  // don't support batching the requested batch size must be 1.
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

  provider->reset(new GRPCPredictRequestProvider(id, request));
  return tensorflow::Status::OK();
}

size_t GRPCPredictRequestProvider::batchSize() const {
  return req.batch_size();
}

size_t GRPCPredictRequestProvider::inputSize() const {
  return req.inputs().size();
}

void GRPCPredictRequestProvider::visitInputs(InputVisitor visitor) const {
  for (auto& i : req.inputs()) {
    if (!visitor(i.first, i.second))
      break;
  }
}

void GRPCPredictRequestProvider::outputNames(OutputNames& output_names) const {
  std::set<absl::string_view> seens;

  for (auto& it : req.output_filter()) {
    if (seens.emplace(it.first).second) {
      output_names.emplace_back(it.first);
    }
  }
}

}  // namespace serving
}  // namespace adlik
