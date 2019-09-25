#include "cub/string/string_view.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

FIXTURE(StringViewTest) {
  TEST("c-style string") {
    // const char* without size.
    auto hello = "hello";
    StringView s20(hello);
    ASSERT_TRUE(s20.data() == hello);
    ASSERT_EQ(5, s20.size());

    StringView s21(hello, 4);
    ASSERT_TRUE(s21.data() == hello);
    ASSERT_EQ(4, s21.size());

    // Not recommended, but valid C++
    StringView s22(hello, 6);
    ASSERT_TRUE(s22.data() == hello);
    ASSERT_EQ(6, s22.size());
  }

  TEST("std::string") {
    std::string hola = "hola";
    StringView s30(hola);
    ASSERT_TRUE(s30.data() == hola.data());
    ASSERT_EQ(4, s30.size());

    // std::string with embedded '\0'.
    hola.push_back('\0');
    hola.append("h2");
    hola.push_back('\0');
    StringView s31(hola);
    ASSERT_TRUE(s31.data() == hola.data());
    ASSERT_EQ(8, s31.size());
  }

  TEST("to std::string explicitly") {
    ASSERT_EQ(std::string(""), std::string(StringView("")));
    ASSERT_EQ(std::string("foo"), std::string(StringView("foo")));
  }
};

}
