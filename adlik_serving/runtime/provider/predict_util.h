// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef SERVINGLTE_RUNTIME_PROVIDER_PREDICT_UTIL_H
#define SERVINGLTE_RUNTIME_PROVIDER_PREDICT_UTIL_H

#include <memory>
#include <string>

#include "adlik_serving/apis/predict.pb.h"
#include "adlik_serving/framework/domain/auto_model_handle.h"
#include "adlik_serving/framework/domain/model_spec.pb.h"
#include "adlik_serving/runtime/provider/grpc_predict_request_provider.h"
#include "adlik_serving/runtime/provider/grpc_predict_response_provider.h"
#include "cub/base/fwddecl.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

struct ModelHandle;
struct RunOptions;

struct PredictUtil {
  PredictUtil(const RunOptions&, ModelHandle*, const PredictRequest&, PredictResponse&);

  template <typename ModelType>
  tensorflow::Status predict();

private:
  const RunOptions& opts;
  ModelHandle* handle;
  const PredictRequest& req;
  PredictResponse& rsp;
};

///////////////////////////////////////////////////////////////////////////////////

template <typename ModelType>
tensorflow::Status PredictUtil::predict() {
  AutoModelHandle<ModelType> bundle(handle);

  std::unique_ptr<GRPCPredictRequestProvider> request_provider;
  TF_RETURN_IF_ERROR(GRPCPredictRequestProvider::create(req, &request_provider));

  std::unique_ptr<GRPCPredictResponseProvider> response_provider;
  TF_RETURN_IF_ERROR(GRPCPredictResponseProvider::create(req, rsp, &response_provider));

  return bundle->predict(opts, request_provider.get(), response_provider.get());
}

}  // namespace serving
}  // namespace adlik

#endif
