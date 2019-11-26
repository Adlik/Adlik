package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_toolchain", "cc_toolchain_suite")
load(":cc_toolchain_config.bzl", "cc_toolchain_config")

cc_toolchain_suite(
    name = "toolchain",
    toolchains = {
        "aarch64": ":gcc-linux-aarch64",
    },
)

filegroup(
    name = "empty",
    srcs = [],
)

cc_toolchain_config(
    name = "aarch64_toolchain_config",
    cpu = "aarch64",
)

cc_toolchain(
    name = "gcc-linux-aarch64",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    dynamic_runtime_lib = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    static_runtime_lib = ":empty",
    strip_files = ":empty",
    supports_param_files = 1,
    toolchain_config = ":aarch64_toolchain_config",
    toolchain_identifier = "aarch64",
)
