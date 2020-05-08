load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "dialect",
    hdrs = [
        "dialect.h",
    ],
    deps = [
    ],
)

cc_library(
    name = "csv_reader",
    srcs = [
        "csv_reader.cc",
    ],
    hdrs = [
        "csv_reader.h",
    ],
    deps = [
        ":dialect",
        "//cub/string:string_view",
    ],
)

cc_library(
    name = "csv_writer",
    srcs = [
        "csv_writer.cc",
    ],
    hdrs = [
        "csv_writer.h",
    ],
    deps = [
        ":dialect",
    ],
)

filegroup(
    name = "test_data",
    srcs = glob([
        "test_data/*.csv",
    ]),
)

cc_library(
    name = "test_util",
    hdrs = [
        "test_util.h",
    ],
    deps = [
        "//cub/env/fs:path",
    ],
)

cc_test(
    name = "csv_test",
    srcs = [
        "csv_reader_test.cc",
        "csv_writer_test.cc",
    ],
    data = [
        ":test_data",
    ],
    deps = [
        ":csv_reader",
        ":csv_writer",
        ":test_util",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)
