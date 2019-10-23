// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/shared_model_handle.h"

namespace adlik {
namespace serving {

SharedModelHandle::SharedModelHandle(std::shared_ptr<ModelHandle> handle) : handle(std::move(handle)) {
}

cub::AnyPtr SharedModelHandle::model() {
  return handle->model();
}

}  // namespace serving
}  // namespace adlik
