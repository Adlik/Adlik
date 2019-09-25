load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "fs",
    hdrs = [
        "file_system.h",
    ],
    deps = [
        "//cub/base:status",
        "//cub/dci",
        "//cub/env",
    ],
)

cc_library(
    name = "uri",
    srcs = ["uri.cc"],
    hdrs = ["uri.h"],
    deps = [
        "//cub/string:scanner",
        "//cub/string:string_view",
    ],
)

cc_test(
    name = "uri_test",
    srcs = ["uri_test.cc"],
    deps = [
        ":uri",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "path",
    srcs = ["path.cc"],
    hdrs = ["path.h"],
    deps = [
        ":uri",
        "//cub/string:string_view",
    ],
)

cc_test(
    name = "path_test",
    srcs = ["path_test.cc"],
    deps = [
        ":path",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)
