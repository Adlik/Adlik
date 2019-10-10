// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_UTIL_DEFAULT_MODEL_FACTORY_H
#define ADLIK_SERVING_RUNTIME_UTIL_DEFAULT_MODEL_FACTORY_H

#include "adlik_serving/framework/manager/model_factory.h"

namespace adlik {
namespace serving {

template <typename T>
struct DefaultModelFactory : ModelFactory {
  void config() {
    // todo
  }

  OVERRIDE(Model* create(const adlik::serving::ModelId& id, const adlik::serving::ModelConfig& config)) {
    return new T(id, config);
  }
};

}  // namespace serving
}  // namespace adlik

#endif
