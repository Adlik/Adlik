// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/algo/search.h"

#include "cut/cut.hpp"

using namespace cum;
using namespace cub;

namespace {
int a[] = {1, 3, 5, 7};
}

FIXTURE(BinarySearchTest){TEST("should find correct when key is in the middle position"){
    int* r = binary_search(std::begin(a), std::end(a), 5);
ASSERT_THAT(*r, eq(5));
}

TEST("should return the end when key not find") {
  ASSERT_THAT(binary_search(std::begin(a), std::end(a), 6), eq(std::end(a)));
}
}
;
