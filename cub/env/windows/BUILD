load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "windows",
    srcs = ["env.cc"],
    deps = [
        "//cub/env/concurrent",
        "//cub/env/fs",
        "//cub/env/time",
    ],
)
