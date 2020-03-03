// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HE26FC613_3D2E_49C8_95D1_67677F86FECD
#define HE26FC613_3D2E_49C8_95D1_67677F86FECD

#include <memory>

#include "adlik_serving/framework/domain/model_handle.h"

namespace adlik {
namespace serving {

template <typename T>
struct AutoModelHandle {
  AutoModelHandle(ModelHandle* handle) : handle(handle) {
  }

  const ModelId& id() const {
    return handle->ROLE(ModelId);
  }

  bool isNull() const {
    return get() == nullptr;
  }

  bool isNotNull() const {
    return !isNull();
  }

  // preconditon: isNotNull()
  T& operator*() {
    return *get();
  }

  // preconditon: isNotNull()
  const T& operator*() const {
    return *get();
  }

  T* operator->() {
    return get();
  }

  const T* operator->() const {
    return get();
  }

  T* get() const {
    return handle ? handle->model().template get<T>() : nullptr;
  }

private:
  // std::unique_ptr<ModelHandle> handle; // Be careful, may cause memory to be freed
  // repeatedly
  ModelHandle* handle;
};

}  // namespace serving
}  // namespace adlik

#endif
