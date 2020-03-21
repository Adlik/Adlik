load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = [
        "//visibility:public",
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
        "//cub/log",
        "//cub/string:str_utils",
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
        "//cub/string:str_utils",
    ],
)
