#include "cub/env/fs/path.h"
#include <cut/cut.hpp>

using namespace cum;

namespace cub {

namespace {

using namespace std::string_literals;

FIXTURE(PathTest) {
  TEST("dir name: fullpath, path is empty") {
    ASSERT_EQ(StringView("hdfs://127.0.0.1:9000"),
        Path("hdfs://127.0.0.1:9000").dirName());
  }

  TEST("dir name: fullpath, path is /data") {
    ASSERT_EQ(StringView("hdfs://127.0.0.1:9000/data"),
        Path("hdfs://127.0.0.1:9000/data/train.csv.tfrecords").dirName());
  }

  TEST("dir name: fullpath, path is root /") {
    ASSERT_EQ(StringView("hdfs://127.0.0.1:9000/"),
        Path("hdfs://127.0.0.1:9000/train.csv.tfrecords").dirName());
  }

  TEST("dir name: no scheme & no host, normal case") {
    ASSERT_EQ(StringView("/hello"), Path("/hello/").dirName());
    ASSERT_EQ(StringView("/"), Path("/hello").dirName());
  }

  TEST("dir name: no scheme & no host, relative path") {
    ASSERT_EQ(StringView("hello"), Path("hello/world").dirName());
    ASSERT_EQ(StringView("hello"), Path("hello/").dirName());
  }

  TEST("dir name: no scheme & no host, dir is empty") {
    ASSERT_EQ(StringView(""), Path("world").dirName());
  }

  TEST("dir name: no scheme & no host, only root directory") {
    ASSERT_EQ(StringView("/"), Path("/").dirName());
  }

  TEST("dir name: no scheme & no host: empty path") {
    ASSERT_EQ(StringView(""), Path("").dirName());
  }

  TEST("base name: empty base name") {
    ASSERT_EQ(StringView(""), Path("/hello/").baseName());
    ASSERT_EQ(StringView(""), Path("hello/").baseName());
    ASSERT_EQ(StringView(""), Path("/").baseName());
    ASSERT_EQ(StringView(""), Path("").baseName());
  }

  TEST("base name: non empty base name") {
    ASSERT_EQ(StringView("hello"), Path("/hello").baseName());
    ASSERT_EQ(StringView("world"), Path("hello/world").baseName());
    ASSERT_EQ(StringView("world"), Path("world").baseName());
  }

  TEST("ext name: empty extension") {
    ASSERT_EQ(StringView(""), Path("foo.").extName());
    ASSERT_EQ(StringView(""), Path("").extName());
    ASSERT_EQ(StringView(""), Path("/").extName());
    ASSERT_EQ(StringView(""), Path("foo").extName());
    ASSERT_EQ(StringView(""), Path("foo/").extName());
    ASSERT_EQ(StringView(""), Path("/a/path.bar/to/foo").extName());
  }

  TEST("ext name: non empty extension") {
    ASSERT_EQ(StringView("gif"), Path("foo.gif").extName());
    ASSERT_EQ(StringView("gif"), Path("/a/path/to/foo.gif").extName());
    ASSERT_EQ(StringView("html"), Path("/a/path.bar/to/foo.html").extName());
    ASSERT_EQ(StringView("baz"), Path("/a/path.bar/to/foo.bar.baz").extName());
  }

  TEST("file name: empty file name") {
    ASSERT_EQ(StringView(""), Path("").fileName());
    ASSERT_EQ(StringView(""), Path("/").fileName());
    ASSERT_EQ(StringView(""), Path("foo/").fileName());
  }

  TEST("file name: non empty file name") {
    ASSERT_EQ(StringView("foo"), Path("foo.").fileName());
    ASSERT_EQ(StringView("foo"), Path("foo").fileName());
    ASSERT_EQ(StringView("foo"), Path("/a/path.bar/to/foo").fileName());
    ASSERT_EQ(StringView("foo"), Path("foo.gif").fileName());
    ASSERT_EQ(StringView("foo"), Path("/a/path/to/foo.gif").fileName());
    ASSERT_EQ(StringView("foo"), Path("/a/path.bar/to/foo.html").fileName());
    ASSERT_EQ(StringView("foo.bar"), Path("/a/path.bar/to/foo.bar.baz").fileName());
  }

  TEST("join paths") {
    ASSERT_EQ("/foo/bar"s, paths("/foo", "bar"));
    ASSERT_EQ("foo/bar"s, paths("foo", "bar"));
    ASSERT_EQ("foo/bar"s, paths("foo", "/bar"));
    ASSERT_EQ("/foo/bar"s, paths("/foo", "/bar"));

    ASSERT_EQ("/bar"s, paths("", "/bar"));
    ASSERT_EQ("bar"s, paths("", "bar"));
    ASSERT_EQ("/foo"s, paths("/foo", ""));

    ASSERT_EQ("/foo/bar/baz/blah/blink/biz"s,
        paths("/foo/bar/baz/", "/blah/blink/biz"));
    ASSERT_EQ("/foo/bar/baz/blah"s, paths("/foo", "bar", "baz", "blah"));
  }

};

}

CUM_NS_END
