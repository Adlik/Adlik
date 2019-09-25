#include "cub/env/fs/uri.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

FIXTURE(UriTest) {

  void assertUri(const std::string& str,
      StringView scheme, StringView host, StringView path) {
    Uri uri(str);

    StringView s, h, p;
    uri.get(s, h, p);

    ASSERT_EQ(scheme, s);
    ASSERT_EQ(host,   h);
    ASSERT_EQ(path,   p);

    Uri other(s, h, p);
    ASSERT_EQ(str, other.to_s());
  }

  TEST("no path") {
    assertUri("http://foo", "http", "foo", "");
    assertUri("hdfs://localhost:8020", "hdfs", "localhost:8020", "");
  }

  TEST("no scheme, no path") {
    assertUri("/encrypted/://foo", "", "", "/encrypted/://foo");
    assertUri("/usr/local/foo", "", "", "/usr/local/foo");
    assertUri("a-b:///foo", "", "", "a-b:///foo");
    assertUri(":///foo", "", "", ":///foo");
    assertUri("file:", "", "", "file:");
    assertUri("9dfd:///foo", "", "", "9dfd:///foo");
    assertUri("file:/", "", "", "file:/");
  }

  TEST("no host") {
    assertUri("file:///usr/local/foo", "file", "", "/usr/local/foo");
    assertUri("local.file:///usr/local/foo", "local.file", "",
                     "/usr/local/foo");
  }

  TEST("full path") {
    assertUri("hdfs://localhost:8020/path/to/file",
              "hdfs", "localhost:8020", "/path/to/file");
    assertUri("hdfs://localhost:8020/", "hdfs", "localhost:8020", "/");
  }
};

}
