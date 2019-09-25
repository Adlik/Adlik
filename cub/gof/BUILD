load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "observer",
    hdrs = ["observer.h"],
)

cc_test(
    name = "observer_test",
    srcs = ["observer_test.cc"],
    deps = [
        ":observer",
        "//cub/env/concurrent:loop_thread",
        "//cub/env/posix",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "singleton",
    hdrs = ["singleton.h"],
    deps = ["//cub/base:uncloneable"],
)
