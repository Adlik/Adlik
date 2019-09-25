load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "string_view",
    srcs = ["string_view.cc"],
    hdrs = ["string_view.h"],
)

cc_test(
    name = "string_view_test",
    srcs = ["string_view_test.cc"],
    deps = [
        ":string_view",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "str_utils",
    srcs = ["str_utils.cc"],
    hdrs = ["str_utils.h"],
    deps = [":string_view"],
)

cc_test(
    name = "str_utils_test",
    srcs = ["str_utils_test.cc"],
    deps = [
        ":str_utils",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "str_printf",
    srcs = ["str_printf.cc"],
    hdrs = ["str_printf.h"],
)

cc_test(
    name = "str_printf_test",
    srcs = ["str_printf_test.cc"],
    deps = [
        ":str_printf",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "char_spec",
    srcs = ["char_spec.cc"],
    hdrs = ["char_spec.h"],
)

cc_library(
    name = "scanner",
    srcs = ["scanner.cc"],
    hdrs = ["scanner.h"],
    deps = [
        ":char_spec",
        ":str_utils",
    ],
)

cc_test(
    name = "scanner_test",
    srcs = ["scanner_test.cc"],
    deps = [
        ":scanner",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)
