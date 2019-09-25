#ifndef HE4136C05_23D2_4405_AB1F_C96C3A4F3FDD
#define HE4136C05_23D2_4405_AB1F_C96C3A4F3FDD

#include <functional>

namespace cub {

template <typename T>
inline void hash_combine(std::size_t& seed, const T& t) {
  seed ^= std::hash<T>{}(t) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename Iterator>
inline void hash_range(std::size_t& seed, Iterator first, Iterator last) {
  for (; first != last; ++first) {
    hash_combine(seed, *first);
  }
}

template <typename Container>
inline void hash_range(std::size_t& seed, const Container& c) {
  return hash_range(seed, std::cbegin(c), std::cend(c));
}

}  // namespace cub

#endif
