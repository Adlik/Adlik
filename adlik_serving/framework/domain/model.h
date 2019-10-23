// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H7B663C30_F9B1_45D8_A8D9_5F93CE3F35B5
#define H7B663C30_F9B1_45D8_A8D9_5F93CE3F35B5

#include "adlik_serving/framework/domain/model_handle.h"
#include "adlik_serving/framework/domain/model_loader.h"
#include "adlik_serving/framework/domain/model_state.h"

namespace adlik {
namespace serving {

struct Model : ModelState, ModelLoader {
  Model(const ModelId& id) : ModelState(id) {
  }

  ModelHandle* handle() const {
    return &ROLE(ModelHandle);
  }

private:
  USE_ROLE(ModelHandle);
};

}  // namespace serving
}  // namespace adlik

#endif
