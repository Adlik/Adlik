#ifndef H1BEBC79D_5BA8_1234_AD8C_94F820EC7D5B
#define H1BEBC79D_5BA8_1234_AD8C_94F820EC7D5B

#include <stddef.h>

namespace cub {

namespace details {
template <typename T, bool IsLowBound>
size_t bound(const T* array, size_t len, T key) {
  size_t low = 0;
  size_t high = len - 1;
  size_t mid = 0;

  if (key < array[low]) {
    return low;
  }

  if (key > array[high]) {
    return high;
  }

  while (low <= high) {
    mid = (low + high) >> 1;
    if (key == array[mid]) {
      return mid;
    } else if (key > array[mid]) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }
  return IsLowBound ? high : low;
}
}  // namespace details

template <typename T>
size_t lower_bound(const T* array, size_t len, T key) {
  return details::bound<T, true>(array, len, key);
}

template <typename T>
size_t upper_bound(const T* array, size_t len, T key) {
  return details::bound<T, false>(array, len, key);
}

}  // namespace cub

#endif
