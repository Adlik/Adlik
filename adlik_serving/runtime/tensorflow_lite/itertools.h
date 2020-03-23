// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_ITERTOOLS_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_ITERTOOLS_H

#include <algorithm>

namespace adlik {
namespace serving {
template <class C, class I>
void copyContainer(const C& container, I target) {
  std::copy(std::begin(container), std::end(container), target);
}

template <class C, class I, class F>
void transformContainer(const C& container, I target, F&& func) {
  std::transform(std::begin(container), std::end(container), target, std::forward<F>(func));
}

template <class C, class I>
void transformContainerWithStaticCast(const C& container, I target) {
  using T = std::remove_reference_t<decltype(*target)>;

  std::transform(std::begin(container), std::end(container), target, [](auto value) { return static_cast<T>(value); });
}

namespace itertools {
template <class I, class F>
class MapIterator {
  static_assert(std::is_lvalue_reference<F>::value);

  using MappedType = decltype(std::declval<F>()(*std::declval<I>()));
  using BaseCategory = typename std::iterator_traits<I>::iterator_category;

  I inner;
  F func;

public:
  using difference_type = typename std::iterator_traits<I>::difference_type;
  using value_type = std::remove_reference_t<MappedType>;
  using pointer = typename std::conditional<std::is_reference<MappedType>::value, value_type*, void>::type;
  using reference = MappedType;

  using iterator_category =
      typename std::conditional<std::is_base_of<std::random_access_iterator_tag, BaseCategory>::value,
                                std::random_access_iterator_tag,
                                BaseCategory>::type;

  MapIterator(I inner, F func) : inner(inner), func(func) {
  }

  MapIterator& operator++() {
    ++this->inner;

    return *this;
  }

  auto operator-(MapIterator rhs) const {
    return this->inner - rhs.inner;
  }

  bool operator!=(MapIterator rhs) const {
    return this->inner != rhs.inner;
  }

  reference operator[](difference_type i) const {
    return this->f(this->iter[i]);
  }

  reference operator*() const {
    return this->func(*this->inner);
  }

  auto operator-> () const {
    return pointer{&**this};
  }
};

template <class I, class F>
MapIterator<I, F> make_map_iterator(I&& iter, F&& f) {
  return MapIterator<I, F>{std::forward<I>(iter), std::forward<F>(f)};
}

template <class C, class F>
auto make_map(C&& container, F&& func) {
  class Map {
    C container;
    F func;

  public:
    Map(C&& container, F&& func) : container{std::forward<C>(container)}, func{std::forward<F>(func)} {
    }

    auto begin() {
      return make_map_iterator(this->container.begin(), this->func);
    }

    auto begin() const {
      return make_map_iterator(this->container.begin(), this->func);
    }

    auto end() {
      return make_map_iterator(this->container.end(), this->func);
    }

    auto end() const {
      return make_map_iterator(this->container.end(), this->func);
    }

    auto size() const {
      return this->container.size();
    }
  } result{std::forward<C>(container), std::forward<F>(func)};

  return result;
}
}  // namespace itertools
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_ITERTOOLS_H
