// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H671141B2_B247_540B_8E83_476558110FFE
#define H671141B2_B247_540B_8E83_476558110FFE

#include <type_traits>

#define STATIC_ASSERT(expr) static_assert(expr, #expr)

#define STATIC_ASSERT_TYPE(expect_type, expr) STATIC_ASSERT((std::is_same<expect_type, decltype(expr)>::value))

#define STATIC_ASSERT_SAME_TYPE(expect_type, actual_type) \
  STATIC_ASSERT_TRUE((std::is_same<expect_type, actual_type>::value))

#define STATIC_ASSERT_RVALUE_REF(expr) STATIC_ASSERT_TRUE(std::is_rvalue_reference<decltype(expr)>::value)

#define STATIC_ASSERT_LVALUE_REF(expr) STATIC_ASSERT_TRUE(std::is_lvalue_reference<decltype(expr)>::value)

#endif
