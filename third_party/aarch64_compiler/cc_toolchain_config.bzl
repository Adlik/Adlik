"""
Rule of aarch64 toolchain

"""

load("@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl", "feature", "flag_group", "flag_set", "tool_path")
load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

def _impl(ctx):
    tool_paths = [
        tool_path(
            name = "gcc",
            path = "/usr/bin/aarch64-linux-gnu-gcc",
        ),
        tool_path(
            name = "ld",
            path = "/usr/bin/aarch64-linux-gnu-ld",
        ),
        tool_path(
            name = "ar",
            path = "/usr/bin/aarch64-linux-gnu-ar",
        ),
        tool_path(
            name = "cpp",
            path = "/usr/bin/aarch64-linux-gnu-cpp",
        ),
        tool_path(
            name = "gcov",
            path = "/usr/bin/aarch64-linux-gnu-gcov",
        ),
        tool_path(
            name = "nm",
            path = "/usr/bin/aarch64-linux-gnu-nm",
        ),
        tool_path(
            name = "objcopy",
            path = "/usr/bin/aarch64-linux-gnu-objcopy",
        ),
        tool_path(
            name = "objdump",
            path = "/usr/bin/aarch64-linux-gnu-objdump",
        ),
        tool_path(
            name = "strip",
            path = "/usr/bin/aarch64-linux-gnu-strip",
        ),
        tool_path(
            name = "dwp",
            path = "/usr/bin/aarch64-linux-gnu-dwp",
        ),
    ]
    cxx_builtin_include_directories = [
        "/usr/aarch64-linux-gnu/include/c++/7/",
        "/usr/aarch64-linux-gnu/include/c++/7/backward",
        "/usr/aarch64-linux-gnu/include/",
        "/usr/lib/gcc-cross/aarch64-linux-gnu/7/include",
        "/usr/lib/gcc-cross/aarch64-linux-gnu/7/include-fixed",
    ]

    all_link_actions = [
        ACTION_NAMES.cpp_link_executable,
        ACTION_NAMES.cpp_link_dynamic_library,
        ACTION_NAMES.cpp_link_nodeps_dynamic_library,
    ]

    pic_feature = feature(
        name = "pic",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.c_compile, ACTION_NAMES.cpp_compile],
                flag_groups = [
                    flag_group(flags = ["-fPIC"], expand_if_available = "pic"),
                    flag_group(
                        flags = ["-fPIE"],
                        expand_if_not_available = "pic",
                    ),
                ],
            ),
        ],
    )

    determinism_feature = feature(
        name = "determinism",
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.c_compile, ACTION_NAMES.cpp_compile],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-Wno-builtin-macro-redefined",
                            "-D__DATE__=\"redacted\"",
                            "-D__TIMESTAMP__=\"redacted\"",
                            "-D__TIME__=\"redacted\"",
                        ],
                    ),
                ],
            ),
        ],
    )

    supports_pic_feature = feature(name = "supports_pic", enabled = True)

    hardening_feature = feature(
        name = "hardening",
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.c_compile, ACTION_NAMES.cpp_compile],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-U_FORTIFY_SOURCE",
                            "-D_FORTIFY_SOURCE=1",
                            "-fstack-protector",
                        ],
                    ),
                ],
            ),
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_dynamic_library,
                    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
                ],
                flag_groups = [flag_group(flags = ["-Wl,-z,relro,-z,now"])],
            ),
            flag_set(
                actions = [ACTION_NAMES.cpp_link_executable],
                flag_groups = [flag_group(flags = ["-pie", "-Wl,-z,relro,-z,now"])],
            ),
        ],
    )

    supports_dynamic_linker_feature = feature(name = "supports_dynamic_linker", enabled = True)

    warnings_feature = feature(
        name = "warnings",
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.c_compile, ACTION_NAMES.cpp_compile],
                flag_groups = [
                    flag_group(
                        flags = ["-Wall"] + ctx.attr.host_compiler_warnings,
                    ),
                ],
            ),
        ],
    )

    dbg_feature = feature(
        name = "dbg",
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.c_compile, ACTION_NAMES.cpp_compile],
                flag_groups = [flag_group(flags = ["-g3", "-ggdb"])],
            ),
        ],
        implies = ["common"],
    )

    disable_assertions_feature = feature(
        name = "disable-assertions",
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.c_compile, ACTION_NAMES.cpp_compile],
                flag_groups = [flag_group(flags = ["-DNDEBUG"])],
            ),
        ],
    )

    fastbuild_feature = feature(name = "fastbuild", implies = ["common"])

    linker_bin_path_feature = feature(
        name = "linker-bin-path",
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [flag_group(flags = ["-B" + ctx.attr.linker_bin_path])],
            ),
        ],
    )

    opt_feature = feature(
        name = "opt",
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.c_compile, ACTION_NAMES.cpp_compile],
                flag_groups = [
                    flag_group(
                        flags = ["-ggdb", "-O2", "-ffunction-sections", "-fdata-sections"],
                    ),
                ],
            ),
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_dynamic_library,
                    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
                    ACTION_NAMES.cpp_link_executable,
                ],
                flag_groups = [flag_group(flags = ["-Wl,--gc-sections"])],
            ),
        ],
        implies = ["common", "disable-assertions"],
    )

    frame_pointer_feature = feature(
        name = "frame-pointer",
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.c_compile, ACTION_NAMES.cpp_compile],
                flag_groups = [flag_group(flags = ["-fno-omit-frame-pointer"])],
            ),
        ],
    )

    build_id_feature = feature(
        name = "build-id",
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [
                    flag_group(
                        flags = ["-Wl,--build-id=md5", "-Wl,--hash-style=gnu"],
                    ),
                ],
            ),
        ],
    )

    stdlib_feature = feature(
        name = "stdlib",
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [flag_group(flags = [
                    "-static",
                    "-lstdc++",
                    "-lm",
                    "-lpthread",
                    "-ldl",
                    "-lrt",
                ])],
            ),
        ],
    )

    no_stripping_feature = feature(name = "no_stripping")

    alwayslink_feature = feature(
        name = "alwayslink",
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_dynamic_library,
                    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
                    ACTION_NAMES.cpp_link_executable,
                ],
                flag_groups = [flag_group(flags = ["-Wl,-no-as-needed"])],
            ),
        ],
    )

    no_canonical_prefixes_feature = feature(
        name = "no-canonical-prefixes",
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.c_compile,
                    ACTION_NAMES.cpp_compile,
                    ACTION_NAMES.cpp_link_executable,
                    ACTION_NAMES.cpp_link_dynamic_library,
                    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-no-canonical-prefixes",
                        ] + ctx.attr.extra_no_canonical_prefixes_flags,
                    ),
                ],
            ),
        ],
    )

    has_configured_linker_path_feature = feature(name = "has_configured_linker_path")

    copy_dynamic_libraries_to_binary_feature = feature(name = "copy_dynamic_libraries_to_binary")

    cpp17_feature = feature(
        name = "c++17",
        flag_sets = [
            flag_set(
                actions = [ACTION_NAMES.cpp_compile],
                flag_groups = [flag_group(flags = ["-std=c++17"])],
            ),
        ],
    )

    lld_feature = feature(
        name = "lld",
        flag_sets = [
            flag_set(
                actions = all_link_actions,
                flag_groups = [flag_group(flags = [
                    "-fuse-ld=gold",
                ])],
            ),
        ],
    )

    common_feature = feature(
        name = "common",
        implies = [
            "stdlib",
            # "c++17",
            "determinism",
            "alwayslink",
            "hardening",
            "warnings",
            "frame-pointer",
            "build-id",
            "no-canonical-prefixes",
            "linker-bin-path",
            "lld",
        ],
    )

    features = [
        # cpp17_feature,
        stdlib_feature,
        determinism_feature,
        alwayslink_feature,
        pic_feature,
        hardening_feature,
        warnings_feature,
        frame_pointer_feature,
        build_id_feature,
        no_canonical_prefixes_feature,
        disable_assertions_feature,
        linker_bin_path_feature,
        common_feature,
        opt_feature,
        fastbuild_feature,
        dbg_feature,
        supports_dynamic_linker_feature,
        supports_pic_feature,
        lld_feature,
    ]

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = "aarch64-linux-gnu",
        host_system_name = "aarch64",
        target_system_name = "aarch64",
        target_cpu = "aarch64",
        target_libc = "aarch64",
        compiler = "compiler",
        abi_version = "aarch64",
        abi_libc_version = "aarch64",
        tool_paths = tool_paths,
        cxx_builtin_include_directories = cxx_builtin_include_directories,
        features = features,
    )

cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {
        "cpu": attr.string(mandatory = True, values = ["aarch64"]),
        "builtin_include_directories": attr.string_list(),
        "extra_no_canonical_prefixes_flags": attr.string_list(),
        "host_compiler_path": attr.string(),
        "host_compiler_prefix": attr.string(),
        "host_compiler_warnings": attr.string_list(),
        "host_unfiltered_compile_flags": attr.string_list(),
        "linker_bin_path": attr.string(),
        "target_cpu": attr.string(),
        "target_system_name": attr.string(),
        "toolchain_identifier": attr.string(),
        "extra_features": attr.string_list(),
    },
    provides = [CcToolchainConfigInfo],
)
