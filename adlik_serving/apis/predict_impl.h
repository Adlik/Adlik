// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_APIS_PREDICT_IMPL_H_
#define ADLIK_SERVING_APIS_PREDICT_IMPL_H_

#include "cub/dci/role.h"
#include "tensorflow/core/lib/core/status.h"

namespace adlik {
namespace serving {

struct ServingStore;
struct ModelStore;
struct ManagedStore;
struct PredictRequest;
struct PredictResponse;
struct RunOptions;

struct PredictImpl {
  virtual ~PredictImpl() = default;

  tensorflow::Status predict(const RunOptions&, const PredictRequest&, PredictResponse&);

  tensorflow::Status predict(const PredictRequest&, PredictResponse&);

private:
  USE_ROLE(ServingStore);
  USE_ROLE(ModelStore);
  USE_ROLE(ManagedStore);
};

}  // namespace serving
}  // namespace adlik

#endif /* APIS_PREDICT_IMPL_H_ */
