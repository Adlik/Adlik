load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "posix",
    srcs = [
        "env.cc",
        "posix_concurrent.cc",
        "posix_filesystem.cc",
        "posix_time.cc",
    ],
    hdrs = [
        "posix_concurrent.h",
        "posix_filesystem.h",
        "posix_time.h",
    ],
    linkopts = ["-lpthread",
    "-lstdc++fs"],
    deps = [
        "//cub/base:assertions",
        "//cub/env/concurrent",
        "//cub/env/fs",
        "//cub/env/fs:path",
        "//cub/env/time",
        "//cub/string:string_view",
    ],
)

cc_library(
    name = "byte_order",
    hdrs = ["byte_order.h"],
    deps = ["//cub/env"],
)

cc_library(
    name = "library_loader",
    hdrs = ["library_loader.h"],
    deps = ["//cub/env"],
)

cc_library(
    name = "type_name",
    srcs = ["type_name.cc"],
    hdrs = ["type_name.h"],
    deps = ["//cub/env"],
)

cc_test(
    name = "type_name_test",
    srcs = ["type_name_test.cc"],
    deps = [
        ":type_name",
        "//cub/env/posix",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)
