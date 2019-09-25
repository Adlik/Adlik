load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "any_ptr",
    hdrs = ["any_ptr.h"],
)

cc_test(
    name = "any_ptr_test",
    srcs = ["any_ptr_test.cc"],
    deps = [
        ":any_ptr",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "unique_any_ptr",
    hdrs = ["unique_any_ptr.h"],
    deps = [":any_ptr"],
)

cc_test(
    name = "unique_any_ptr_test",
    srcs = ["unique_any_ptr_test.cc"],
    deps = [
        ":unique_any_ptr",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "read_write_ptr",
    hdrs = ["read_write_ptr.h"],
    deps = [
        "//cub/env",
        "//cub/env/concurrent:notification",
        "//cub/env/concurrent:shared_lock",
    ],
)

cc_test(
    name = "read_write_ptr_test",
    srcs = ["read_write_ptr_test.cc"],
    linkopts = ["-lpthread"],
    deps = [
        ":read_write_ptr",
        "//cub/env/posix",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)
