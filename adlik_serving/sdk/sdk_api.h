// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_SDK_SDK_API_H
#define ADLIK_SERVING_SDK_SDK_API_H

#include "cub/base/status.h"

namespace adlik {
namespace serving {

struct PredictRequest;
struct PredictResponse;

cub::Status start(int argc, const char** argv);
cub::Status spredict(const PredictRequest&, PredictResponse&);

}  // namespace serving
}  // namespace adlik

#endif
