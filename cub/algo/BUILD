load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "bits",
    hdrs = ["bits.h"],
)

cc_test(
    name = "bits_test",
    srcs = ["bits_test.cc"],
    deps = [
        ":bits",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "bound",
    hdrs = ["bound.h"],
)

cc_test(
    name = "bound_test",
    srcs = ["bound_test.cc"],
    deps = [
        ":bound",
        "//cub/array:array_size",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "loop",
    hdrs = ["loop.h"],
)

cc_library(
    name = "max_min",
    hdrs = ["max_min.h"],
)

cc_library(
    name = "range",
    hdrs = ["range.h"],
)

cc_test(
    name = "range_test",
    srcs = ["range_test.cc"],
    deps = [
        ":range",
        "//cub/base:static_assert",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "search",
    hdrs = ["search.h"],
)

cc_test(
    name = "search_test",
    srcs = ["search_test.cc"],
    deps = [
        ":search",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)
