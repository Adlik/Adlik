#include "cub/env/posix/type_name.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

using namespace std::string_literals;

FIXTURE(TypeNameTest) {
  TEST("primitive type") {
    ASSERT_EQ("int"s, TypeName<int>::value());
  }

  struct Dummy {};

  TEST("self-defined type") {
    ASSERT_EQ("cub::TypeNameTest::Dummy"s, TypeName<Dummy>::value());
  }
};

}
