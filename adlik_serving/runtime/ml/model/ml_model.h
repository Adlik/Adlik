// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_MODEL_ML_MODEL_H_
#define ADLIK_SERVING_RUNTIME_ML_MODEL_ML_MODEL_H_

#include <memory>
#include <utility>
#include <vector>

#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "cub/base/status_wrapper.h"
#include "cub/env/concurrent/mutex.h"

namespace adlik {
namespace serving {

struct CreateTaskResponse;
struct CreateTaskRequest;
struct ModelConfig;
struct ModelId;

}  // namespace serving
}  // namespace adlik

namespace ml_runtime {

struct MLModel {
  static cub::Status create(const adlik::serving::ModelConfig&,
                            const adlik::serving::ModelId&,
                            std::unique_ptr<MLModel>*);

  MLModel() = default;
  MLModel(MLModel&&) = delete;
  MLModel(const MLModel&) = delete;

  cub::StatusWrapper run(const adlik::serving::CreateTaskRequest&, adlik::serving::CreateTaskResponse&);

private:
  cub::Status init(const adlik::serving::ModelConfig& config, const adlik::serving::ModelId& model_id);

  enum State { AVAILABLE = 0, UNAVAILABLE = 1 };

  std::vector<std::pair<State, std::unique_ptr<Algorithm>>> runners;
  cub::Mutex mutex;
};

}  // namespace ml_runtime

#endif
