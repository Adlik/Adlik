load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "notification",
    srcs = ["notification.cc"],
    hdrs = ["notification.h"],
    deps = [
        ":condition_variable",
    ],
)

cc_library(
    name = "mutex",
    srcs = ["mutex.cc"],
    hdrs = ["mutex.h"],
    deps = [
        "@nsync//:nsync_cpp",
    ],
)

cc_library(
    name = "auto_lock",
    hdrs = ["auto_lock.h"],
    deps = [
        ":mutex",
    ],
)

cc_library(
    name = "shared_lock",
    hdrs = ["shared_lock.h"],
    deps = [
        ":mutex",
    ],
)

cc_library(
    name = "condition_variable",
    srcs = ["condition_variable.cc"],
    hdrs = ["condition_variable.h"],
    deps = [
        ":auto_lock",
    ],
)

cc_library(
    name = "concurrent",
    srcs = [],
    hdrs = [
        "concurrent.h",
        "thread.h",
    ],
    deps = [
        "//cub/dci",
        "//cub/env",
    ],
)

cc_library(
    name = "loop_thread",
    srcs = ["loop_thread.cc"],
    hdrs = ["loop_thread.h"],
    deps = [
        ":concurrent",
        ":notification",
        "//cub/base/uncloneable",
        "//cub/env/posix",
        "//cub/env/time",
    ],
)

cc_test(
    name = "loop_thread_test",
    srcs = [
        "loop_thread_test.cc",
    ],
    deps = [
        ":loop_thread",
        "//cub/env/posix",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "thread_pool",
    srcs = ["thread_pool.cc"],
    hdrs = ["thread_pool.h"],
    deps = [
        ":concurrent",
        "//cub/base/uncloneable",
        "//cub/env",
        "//third_party/eigen3",
    ],
)
