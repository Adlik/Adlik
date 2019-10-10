// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/manager/run_options.h"
#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "adlik_serving/runtime/provider/predict_response_provider.h"
#include "adlik_serving/runtime/util/default_runtime_helper.h"

namespace sample {

// A no scheduler runtime (default) just implements "predict" interface, and
// don't support batching request.

namespace {

struct MyModel {
  static cub::Status create(const adlik::serving::ModelConfig& config,
                            const adlik::serving::ModelId& model_id,
                            std::unique_ptr<MyModel>* bundle) {
    // todo: create a custom model

    return cub::Success;
  }

  tensorflow::Status predict(const adlik::serving::RunOptions& opts,
                             const adlik::serving::PredictRequestProvider* req,
                             adlik::serving::PredictResponseProvider* rsp) {
    // todo: do predict

    return tensorflow::Status::OK();
  }
};

}  // namespace

DEFINE_DEFAULT_RUNTIME(NoScheduler, MyModel);

}  // namespace sample
