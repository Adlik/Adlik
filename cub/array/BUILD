load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "array_size",
    hdrs = ["array_size.h"],
)

cc_library(
    name = "array",
    hdrs = ["array.h"],
    deps = [
        "//cub/base:comparator",
        "//cub/base:placement",
        "//cub/base:static_assert",
    ],
)

cc_test(
    name = "array_test",
    srcs = ["array_test.cc"],
    deps = [
        ":array",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)
