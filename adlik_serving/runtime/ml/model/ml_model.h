// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_MODEL_ML_MODEL_H_
#define ADLIK_SERVING_RUNTIME_ML_MODEL_ML_MODEL_H_

#include <memory>

#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/domain/model_id.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "cub/base/status_wrapper.h"

namespace adlik {
namespace serving {

struct CreateTaskResponse;
struct CreateTaskRequest;

}  // namespace serving
}  // namespace adlik

namespace ml_runtime {

struct MLModel {
  static cub::Status create(const adlik::serving::ModelConfig&,
                            const adlik::serving::ModelId&,
                            std::unique_ptr<MLModel>*);

  MLModel(const adlik::serving::ModelConfig& config, const adlik::serving::ModelId& model_id);

  cub::StatusWrapper run(const adlik::serving::CreateTaskRequest&, adlik::serving::CreateTaskResponse&);

private:
  cub::Status init();

  adlik::serving::ModelConfig config;
  adlik::serving::ModelId model_id;
  std::unique_ptr<Algorithm> algorithm;
};

}  // namespace ml_runtime

#endif
