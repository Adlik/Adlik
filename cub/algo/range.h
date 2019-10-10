// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HA980B189_62DA_454B_B213_0905ED542E61
#define HA980B189_62DA_454B_B213_0905ED542E61

#include <algorithm>
#include <utility>

namespace cub {

#define RANG(c) std::begin(std::forward<Container>(c)), std::end(std::forward<Container>(c))

template <typename Container, class P>
inline bool all_of(Container&& c, P pred) {
  return std::all_of(RANG(c), pred);
}

template <typename Container, class P>
inline bool any_of(Container&& c, P pred) {
  return std::any_of(RANG(c), pred);
}

template <typename Container, class P>
inline auto find_if(Container&& c, P pred) -> decltype(std::begin(std::forward<Container>(c))) {
  return std::find_if(RANG(c), pred);
}

template <typename Container, class T>
inline auto find(Container&& c, const T& t) -> decltype(std::begin(std::forward<Container>(c))) {
  return find_if(std::forward<Container>(c), [&t](const T& e) { return e == t; });
}

template <typename Container, typename Unary>
inline Unary each(Container&& c, Unary f) {
  return std::for_each(RANG(c), f);
}

template <typename Container, typename OutputIterator, typename Unary>
inline OutputIterator map(Container&& c, OutputIterator result, Unary f) {
  return std::transform(RANG(c), result, f);
}

template <class Container, class T, class Binary>
inline T reduce(Container&& container, T init, Binary f) {
  for (auto e : container)
    f(init, e);
  return init;
}

namespace details {
template <class InputIterator, class OutputIterator, class Predicate>
inline OutputIterator do_filter(InputIterator first, InputIterator last, OutputIterator result, Predicate pred) {
  for (; first != last; ++first, ++result)
    if (pred(*first))
      *result = *first;
  return result;
}
}  // namespace details

template <typename Container, typename OutputIterator, typename Predicate>
inline OutputIterator filter(Container&& c, OutputIterator result, Predicate pred) {
  return details::do_filter(RANG(c), result, pred);
}

}  // namespace cub

#endif
