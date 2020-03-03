// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H300C6653_1D9E_486D_A7C7_0A248855D000
#define H300C6653_1D9E_486D_A7C7_0A248855D000

#include <functional>
#include <memory>

#include "cub/env/concurrent/auto_lock.h"
#include "cub/env/concurrent/notification.h"
#include "cub/env/concurrent/shared_lock.h"

namespace cub {

template <typename T>
struct ReadWritePtr {
  using ReadPtr = std::shared_ptr<const T>;
  using OwnPtr = std::unique_ptr<T>;

  ReadWritePtr(OwnPtr = nullptr);

  OwnPtr update(OwnPtr);
  ReadPtr get() const;

private:
  struct RelSharedPtr;

  mutable cub::Mutex mu;
  std::unique_ptr<RelSharedPtr> obj;
};

template <typename T>
struct ReadWritePtr<T>::RelSharedPtr {
  using ReadPtr = std::shared_ptr<const T>;
  using OwnedPtr = std::unique_ptr<T>;

  explicit RelSharedPtr(OwnedPtr object)
      : obj(std::move(object)), readObj(obj.get(), std::bind(&Notification::notify, &releasable)) {
  }

  ~RelSharedPtr() {
    blockingRelease();
  }

  ReadPtr ref() const {
    return readObj;
  }

  OwnedPtr blockingRelease() {
    readObj = nullptr;

    if (obj != nullptr) {
      releasable.wait();
    }

    return std::move(obj);
  }

private:
  Notification releasable;
  OwnedPtr obj;
  ReadPtr readObj;
};

template <typename T>
ReadWritePtr<T>::ReadWritePtr(OwnPtr ptr) : obj(std::make_unique<RelSharedPtr>(std::move(ptr))) {
}

template <typename T>
auto ReadWritePtr<T>::update(OwnPtr object) -> OwnPtr {
  auto local = std::make_unique<RelSharedPtr>(std::move(object));
  {
    cub::AutoLock lock(mu);
    std::swap(obj, local);
  }
  return local->blockingRelease();
}

template <typename T>
auto ReadWritePtr<T>::get() const -> ReadPtr {
  cub::SharedLock lock(mu);
  return obj->ref();
}

}  // namespace cub

#endif
