#include "cub/string/str_printf.h"
#include "cut/cut.hpp"

using namespace cub;
using namespace cum;

namespace cub {

using namespace std::literals::string_literals;

FIXTURE(StringPrintfTest) {
  TEST("string") {
    std::string value("Hello");
    appendf(value, " %s", "World");
    ASSERT_EQ("Hello World"s, value);
  }

  TEST("int") {
    std::string value("Hello");
    appendf(value, " %d", 123);
    ASSERT_EQ("Hello 123"s, value);
  }

  TEST("empty string:1") {
    ASSERT_EQ(""s, stringprintf("%s", ""s.c_str()));
    ASSERT_EQ(""s, stringprintf("%s", ""));
  }

  TEST("empty string:2") {
    std::string value("Hello");
    const char* empty = "";
    appendf(value, "%s", empty);
    ASSERT_EQ("Hello"s, value);
  }

  TEST("empty string:3") {
    std::string value("Hello");
    appendf(value, "%s", "");
    ASSERT_EQ("Hello"s, value);
  }

  TEST("support % and $ format in unix") {
    ASSERT_EQ("123hello w"s, stringprintf("%3$d%2$s %1$c", 'w', "hello", 123));
  }
};

}
