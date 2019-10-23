// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H3D7A9209_E17B_4192_A623_6C76D0567024
#define H3D7A9209_E17B_4192_A623_6C76D0567024

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace cub {

template <typename... Args>
struct Observer {
  using Notifier = std::function<void(Args...)>;

  Observer(Notifier f) : impl(std::make_shared<Impl>(std::move(f))) {
  }

  ~Observer() {
    if (impl != nullptr) {
      impl->orphan();
    }
  }

  Notifier notifier() const {
    auto other = impl;
    return [other](Args... args) { other->notify(std::forward<Args>(args)...); };
  }

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

template <typename... Args>
struct Observer<Args...>::Impl {
  explicit Impl(Notifier f) : f(std::move(f)) {
  }

  bool orphaned() const {
    std::lock_guard<std::mutex> lock(mu);
    return f == nullptr;
  }

  void orphan() {
    std::lock_guard<std::mutex> lock(mu);
    f = nullptr;
  }

  void notify(Args... args) const {
    std::lock_guard<std::mutex> lock(mu);
    if (f != nullptr) {
      f(std::forward<Args>(args)...);
    }
  }

private:
  mutable std::mutex mu;
  Notifier f;
};

}  // namespace cub

#endif
