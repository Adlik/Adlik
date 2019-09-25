#ifndef H1BEBC79D_5BA8_1123_AD8C_94F820EC7D5B
#define H1BEBC79D_5BA8_1123_AD8C_94F820EC7D5B

#include <stdint.h>
#include <algorithm>

namespace cub {

#define __MIN(a, b) std::min(a, b)
#define __MAX(a, b) std::max(a, b)

#define __BETWEEN(v, min, max) __MAX((min), __MIN((v), (max)))

template <typename T, T MIN_VALUE, T MAX_VALUE>
struct ValueScope {
  static T fix(T value) {
    return std::max(MIN_VALUE, std::min(MAX_VALUE, value));
  }
};

template <typename T, T MIN, T MAX>
inline T __between(T value) {
  return ValueScope<T, MIN, MAX>::convert(value);
}

template <uint8_t MIN, uint8_t MAX>
inline uint8_t between(uint8_t value) {
  return __between<uint8_t, MIN, MAX>(value);
}

template <typename T, T MIN, T MAX>
inline bool __isBetween(T value) {
  return (value >= MIN) && (value <= MAX);
}

template <uint8_t MIN, uint8_t MAX>
inline bool isBetween(uint8_t value) {
  return __isBetween<uint8_t, MIN, MAX>(value);
}

}  // namespace cub

#endif
