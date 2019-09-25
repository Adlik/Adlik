load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "option",
    srcs = ["option.cc"],
    hdrs = ["option.h"],
    deps = [
        "//cub/base:keywords",
        "//cub/string:scanner",
        "//cub/string:str_printf",
        "//cub/string:str_utils",
        "//cub/string:string_view",
    ],
)

cc_library(
    name = "option_set",
    srcs = ["option_set.cc"],
    hdrs = ["option_set.h"],
    deps = [
        ":option",
    ],
)

cc_test(
    name = "option_set_test",
    srcs = ["option_set_test.cc"],
    deps = [
        ":option_set",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "program_options",
    srcs = ["program_options.cc"],
    hdrs = ["program_options.h"],
    deps = [
        ":option_set",
    ],
)

cc_library(
    name = "option_subscriber",
    hdrs = ["option_subscriber.h"],
    deps = [
        "//cub/dci",
    ],
)
