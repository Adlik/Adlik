load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "dci",
    hdrs = [
        "interp/decl_role_interp.h",
        "interp/has_role_interp.h",
        "interp/impl_role_interp.h",
        "interp/roles_undef.h",
        "role.h",
    ],
    deps = [
        ":unknown",
        "//cub/base:keywords",
    ],
)

cc_test(
    name = "dci_test",
    srcs = [
        "has_role_test.cc",
        "role_test.cc",
    ],
    deps = [
        ":dci",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)

cc_library(
    name = "unknown",
    hdrs = [
        "interface_def.h",
        "unknown.h",
    ],
)
