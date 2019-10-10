// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/provider/predict_util.h"

namespace adlik {
namespace serving {

PredictUtil::PredictUtil(const RunOptions& opts, ModelHandle* handle, const PredictRequest& req, PredictResponse& rsp)
    : opts(opts), handle(handle), req(req), rsp(rsp) {
}

}  // namespace serving
}  // namespace adlik
