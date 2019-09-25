#include "cut/cut.hpp"
#include "cub/algo/range.h"
#include "cub/base/static_assert.h"
#include <type_traits>

using namespace cum;

namespace {
  int arr[] = {-1, -2, 3, 4};
  std::vector<int> v1 {-1, -2, 3, 4};
  const std::vector<int> v2 {-1, -2, 3, 4};
}

FIXTURE(AlgorithmTest) {
  TEST("find support non const container") {
    auto found = cub::find(v1, 3);

    STATIC_ASSERT_TYPE(std::vector<int>::iterator, found);
    ASSERT_THAT(*found, is(3));
  }

  TEST("find support const container") {
    auto found = cub::find(v2, 3);

    STATIC_ASSERT_TYPE(std::vector<int>::const_iterator, found);
    ASSERT_THAT(*found, is(3));
  }

  TEST("find_if support non const container") {
    auto found = cub::find_if(v1, [](int e) {return e > 0;});

    STATIC_ASSERT_TYPE(std::vector<int>::iterator, found);
    ASSERT_THAT(*found, is(3));
  }

  TEST("find_if support const container") {
    auto found = cub::find_if(v2, [](int e) {return e > 0;});

    STATIC_ASSERT_TYPE(std::vector<int>::const_iterator, found);
    ASSERT_THAT(*found, is(3));
  }

  TEST("map") {
    std::vector<int> v;
    cub::map(v1, std::back_inserter(v), [](int e) {return e > 0 ? e : -e;});

    ASSERT_THAT(v[0], is(1));
    ASSERT_THAT(v[1], is(2));
    ASSERT_THAT(v[2], is(3));
    ASSERT_THAT(v[3], is(4));
  }

  TEST("reduce") {
    auto sum = cub::reduce(arr, 0, [](int& sum, int e) {sum += e;});

    ASSERT_THAT(sum, is(4));
  }

  TEST("filter") {
    std::vector<int> v;
    cub::filter(arr, std::back_inserter(v), [](int e) {return e > 0;});

    ASSERT_THAT(v[0], is(3));
    ASSERT_THAT(v[1], is(4));
  }

  TEST("each") {
    auto sum = 0;
    cub::each(arr, [&sum](int e) {sum += e;});

    ASSERT_THAT(sum, is(4));
  }
};
