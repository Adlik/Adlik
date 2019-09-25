load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "list",
    hdrs = [
        "apr_ring.h",
        "list.h",
        "list_elem.h",
    ],
)

cc_test(
    name = "list_test",
    srcs = ["list_test.cc"],
    copts = [
        "-Wno-invalid-offsetof",
        "-Iexternal/xunit_cut/include",
    ],
    deps = [
        ":list",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)
