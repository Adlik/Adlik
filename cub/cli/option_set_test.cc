#include "cub/cli/option_set.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

FIXTURE(ProgOptionsTest) {
  int v1 = 10;
  float v2 = 12.0;
  bool v3 = false;
  std::string v4 = "v4";

  OptionSet options {{
    option("v1", &v1, "integer option"),
    option("v2", &v2, "float option"),
    option("v3", &v3, "boolean option"),
    option("v4", &v4, "string option"),
  }};

  TEST("parse ok") {
    ASSERT_TRUE(options.parse("--v1=20"));
    ASSERT_TRUE(options.parse("--v2=20.0"));
    ASSERT_TRUE(options.parse("--v3=1"));
    ASSERT_TRUE(options.parse("--v4=something"));

    ASSERT_THAT(v1, eq(20));
    ASSERT_THAT(v2, close_to(20.0, 0.001));
    ASSERT_TRUE(v3);
    ASSERT_THAT("something", eq(v4));
  }

  TEST("parse failed: invalid int") {
    ASSERT_FALSE(options.parse("--v1=str"));
  }

  TEST("parse failed: invalid float") {
    ASSERT_FALSE(options.parse("--v2=str"));
  }

  TEST("parse failed: invalid bool") {
    ASSERT_FALSE(options.parse("--v3=true"));
  }
};

}
