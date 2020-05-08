// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HD506C8AC_D465_411C_AF2E_C0543DBE5D22
#define HD506C8AC_D465_411C_AF2E_C0543DBE5D22

#include <memory>

#include "cub/mem/any_ptr.h"

namespace cub {

struct UniqueAnyPtr {
  UniqueAnyPtr() noexcept {
  }

  UniqueAnyPtr(std::nullptr_t) noexcept : UniqueAnyPtr() {
  }

  template <typename T>
  UniqueAnyPtr(std::unique_ptr<T> ptr) noexcept : p(ptr.release()), del(deleter<T>()) {
  }

  ~UniqueAnyPtr() noexcept {
    del(p);
  }

  UniqueAnyPtr(const UniqueAnyPtr& other) = delete;
  UniqueAnyPtr& operator=(const UniqueAnyPtr& other) = delete;

  UniqueAnyPtr(UniqueAnyPtr&& other) noexcept {
    swap(other);
  }

  UniqueAnyPtr& operator=(UniqueAnyPtr&& other) noexcept {
    swap(other);
    return *this;
  }

  template <typename T>
  T* get() const noexcept {
    return p.get<T>();
  }

  const AnyPtr& any_ptr() const noexcept {
    return p;
  }

  void swap(UniqueAnyPtr& other) noexcept {
    using ::std::swap;
    swap(p, other.p);
    swap(del, other.del);
  }

private:
  using Deleter = void (*)(AnyPtr ptr);

  template <typename T>
  static Deleter deleter() {
    return [](AnyPtr ptr) { delete ptr.get<T>(); };
  }

  static Deleter nop() {
    return [](AnyPtr ptr) {};
  }

private:
  AnyPtr p = nullptr;
  Deleter del = nop();
};

}  // namespace cub

#endif
