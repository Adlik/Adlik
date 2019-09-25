#ifndef H1BEBC79D_5BA8_2341_AD8C_94F820EC7D5B
#define H1BEBC79D_5BA8_2341_AD8C_94F820EC7D5B

#include <algorithm>

namespace cub {

template <typename Iterator, typename T>
Iterator binary_search(Iterator first, Iterator last, const T& val) {
  auto iter = std::lower_bound(first, last, val);
  if (iter != last && *iter == val)
    return iter;
  else
    return last;
}

}  // namespace cub

#endif
