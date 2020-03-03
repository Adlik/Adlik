// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H5C910113_6FE0_4972_8A42_F9CC943F5B74
#define H5C910113_6FE0_4972_8A42_F9CC943F5B74

#include <stddef.h>

#include <type_traits>
#include <utility>

#include "cub/base/comparator.h"
#include "cub/base/placement.h"
#include "cub/base/static_assert.h"

namespace cub {

template <typename T, size_t N>
struct Array {
  using Elem = Placement<T>;

  struct Iterator {
    Iterator() : elem(0) {
    }

    Iterator(Elem* elem) : elem(elem) {
    }

    Iterator(const Iterator& rhs) : elem(rhs.elem) {
    }

    Iterator& operator=(const Iterator& rhs) {
      elem = rhs.elem;
      return *this;
    }

    INLINE_EQUALS(Iterator) {
      return (elem == rhs.elem);
    }

    void reset() {
      elem = 0;
    }

    bool isNull() const {
      return elem == 0;
    }

    T* operator->() {
      return getValue();
    }

    T& operator*() {
      return *getValue();
    }

    T* getValue() const {
      return &(elem->getRef());
    }

    Iterator operator++(int) {
      Iterator i = *this;
      elem++;
      return i;
    }

    Iterator& operator++() {
      elem++;
      return *this;
    }

  private:
    Elem* elem;
  };

  Array() {
    STATIC_ASSERT(N > 0);
    STATIC_ASSERT(std::is_default_constructible<T>::value);
    for (size_t i = 0; i < N; i++) {
      new (elems[i].alloc()) T();
    }
  }

  template <typename... ARGS>
  Array(ARGS&&... args) {
    for (size_t i = 0; i < N; ++i) {
      new (elems[i].alloc()) T(std::forward<ARGS>(args)...);
    }
  }

  size_t size() const {
    return N;
  }

  bool empty() const {
    return size() == 0;
  }

  const T& operator[](size_t i) const {
    return elems[i].getRef();
  }

  T& operator[](size_t i) {
    return elems[i].getRef();
  }

  template <typename... ARGS>
  void emplace(size_t i, ARGS&&... args) {
    if (i >= N)
      return;
    elems[i].destroy();
    new (elems[i].alloc()) T(std::forward<ARGS>(args)...);
  }

  Iterator begin() {
    return {&elems[0]};
  }

  Iterator end() {
    return {&elems[0] + N};
  }

private:
  Placement<T> elems[N];
};

/////////////////////////////////////////////////////////////
#define ARRAY_FOREACH_FROM(i, from, items) for (auto i = from; i != items.end(); ++i)

#define ARRAY_FOREACH(i, items) ARRAY_FOREACH_FROM(i, items.begin(), items)

}  // namespace cub

#endif
