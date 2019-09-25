load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "concurrent_executor",
    srcs = ["concurrent_executor.cc"],
    hdrs = [
        "concurrent_executor.h",
        "executor.h",
    ],
    deps = [
        "//cub/env/concurrent:thread_pool",
    ],
)

cc_library(
    name = "simple_executor",
    srcs = ["simple_executor.cc"],
    hdrs = [
        "executor.h",
        "simple_executor.h",
    ],
    deps = [
        "//cub/env/concurrent",
    ],
)
