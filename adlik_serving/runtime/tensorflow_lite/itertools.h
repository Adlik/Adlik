// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_ITERTOOLS_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_ITERTOOLS_H

#include <algorithm>

namespace adlik {
namespace serving {
template <class C, class I>
void copyContainer(const C& container, I target) {
  std::copy(std::cbegin(container), std::cend(container), target);
}

template <class C, class I, class F>
void transformContainer(const C& container, I target, F&& func) {
  std::transform(std::cbegin(container), std::cend(container), target, std::forward<F>(func));
}

template <class C, class I>
void transformContainerWithStaticCast(const C& container, I target) {
  using T = std::remove_reference_t<decltype(*target)>;

  std::transform(
      std::cbegin(container), std::cend(container), target, [](auto value) { return static_cast<T>(value); });
}
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_ITERTOOLS_H
