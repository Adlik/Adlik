load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")
load("@rules_python//python:defs.bzl", "py_binary")

genrule(
    name = "bazel-actions-proto-file",
    srcs = ["@bazel_extra_actions_base_proto//file"],
    outs = ["extra_actions_base.proto"],
    cmd = "cp $< $@",
)

py_proto_library(
    name = "bazel-actions-py-proto",
    srcs = [":extra_actions_base.proto"],
)

py_binary(
    name = "dump-cpp-compile-command",
    srcs = ["tools/dump-cpp-compile-command.py"],
    visibility = ["//visibility:public"],
    deps = [":bazel-actions-py-proto"],
)

extra_action(
    name = "dump-cpp-compile-command-action",
    cmd = "$(location :dump-cpp-compile-command) < $(EXTRA_ACTION_FILE) > $(output compile-command-$(ACTION_ID).json)",
    out_templates = ["compile-command-$(ACTION_ID).json"],
    tools = [":dump-cpp-compile-command"],
)

action_listener(
    name = "ci-action-listener",
    extra_actions = [":dump-cpp-compile-command-action"],
    mnemonics = ["CppCompile"],
    visibility = ["//visibility:public"],
)
