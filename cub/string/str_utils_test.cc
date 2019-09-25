#include "cub/string/str_utils.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

namespace strutils {

FIXTURE(JoinAndSplitSpec) {
  TEST("basic") {
    ASSERT_TRUE(split("", ",").empty());
    ASSERT_EQ(join(split("a", ","), "|"), "a");
    ASSERT_EQ(join(split(",", ","), "|"), "|");
    ASSERT_EQ(join(split("a,b,c", ","), "|"), "a|b|c");
    ASSERT_EQ(join(split("a,,,b,,c,", ","), "|"), "a|||b||c|");
    ASSERT_EQ(join(split("a!,!b,!c,", ",!"), "|"), "a|||b||c|");
  }
};

FIXTURE(ToInt32Spec) {
  int32_t result;

  TEST("single number") {
    ASSERT_TRUE(to_int32("1", result));
    ASSERT_EQ(1, result);
  }

  TEST("mutil number") {
    ASSERT_TRUE(to_int32("123", result));
    ASSERT_EQ(123, result);
  }

  TEST("with space and negative") {
    ASSERT_TRUE(to_int32(" -123 ", result));
    ASSERT_EQ(-123, result);
  }

  TEST("max int32") {
    ASSERT_TRUE(to_int32("2147483647", result));
    ASSERT_EQ(2147483647, result);
  }

  TEST("min int32") {
    ASSERT_TRUE(to_int32("-2147483648", result));
    ASSERT_EQ(-2147483648, result);
  }

  TEST("invalid") {
    ASSERT_FALSE(to_int32(" 132as ", result));
    ASSERT_FALSE(to_int32(" 132.2 ", result));
    ASSERT_FALSE(to_int32(" -", result));
    ASSERT_FALSE(to_int32("", result));
    ASSERT_FALSE(to_int32("  ", result));
    ASSERT_FALSE(to_int32("123 a", result));
  }

  TEST("overflow") {
    // Overflow
    ASSERT_FALSE(to_int32("2147483648", result));
    ASSERT_FALSE(to_int32("-2147483649", result));
  }

  TEST("defined length") {
    // Check that the StringView's length is respected.
    ASSERT_TRUE(to_int32(StringView("123", 1), result));
    ASSERT_EQ(1, result);

    ASSERT_TRUE(to_int32(StringView(" -123", 4), result));
    ASSERT_EQ(-12, result);

    ASSERT_FALSE(to_int32(StringView(nullptr, 0), result));
  }
};

FIXTURE(StringToUInt32Spec) {
  uint32_t result;

  TEST("single digit") {
    ASSERT_TRUE(to_uint32("0", result));
    ASSERT_EQ(0, result);
    ASSERT_TRUE(to_uint32("1", result));
    ASSERT_EQ(1, result);
  }

  TEST("multi digits") {
    ASSERT_TRUE(to_uint32("123", result));
    ASSERT_EQ(123, result);
  }

  TEST("max uint32") {
    ASSERT_TRUE(to_uint32("4294967295", result));
    ASSERT_EQ(4294967295, result);
  }

  TEST("invalid") {
    ASSERT_FALSE(to_uint32(" 132as ", result));
    ASSERT_FALSE(to_uint32(" 132.2 ", result));
    ASSERT_FALSE(to_uint32(" -", result));
    ASSERT_FALSE(to_uint32("", result));
    ASSERT_FALSE(to_uint32("  ", result));
    ASSERT_FALSE(to_uint32("123 a", result));
    ASSERT_FALSE(to_uint32("123 456", result));
  }

  TEST("overflow") {
    ASSERT_FALSE(to_uint32("4294967296", result));
    ASSERT_FALSE(to_uint32("-1", result));
  }

  TEST("defined length") {
    ASSERT_TRUE(to_uint32(StringView("123", 1), result));
    ASSERT_EQ(1, result);

    ASSERT_TRUE(to_uint32(StringView(" 123", 3), result));
    ASSERT_EQ(12, result);

    ASSERT_FALSE(to_uint32(StringView(nullptr, 0), result));
  }
};

}

}
