"""
Helper wrapper functions
"""

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("@protobuf_archive//:protobuf.bzl", "cc_proto_library", "py_proto_library")

def serving_proto_library(
        name,
        srcs = [],
        has_services = False,
        deps = [],
        visibility = None,
        testonly = 0,
        cc_grpc_version = None,
        cc_api_version = 2):
    """
    Wrapper for proto library.

    Args:
        name:
        srcs:
        has_services:
        deps:
        visibility:
        testonly:
        cc_grpc_version:
        cc_api_version:
    """

    native.filegroup(
        name = name + "_proto_srcs",
        srcs = srcs,
        testonly = testonly,
    )

    use_grpc_plugin = None
    if cc_grpc_version:
        use_grpc_plugin = True
    cc_proto_library(
        name = name,
        srcs = srcs,
        deps = deps,
        cc_libs = ["@protobuf_archive//:protobuf"],
        protoc = "@protobuf_archive//:protoc",
        default_runtime = "@protobuf_archive//:protobuf",
        use_grpc_plugin = use_grpc_plugin,
        testonly = testonly,
        visibility = visibility,
    )

def serving_proto_library_py(
        name,
        proto_library,
        srcs = [],
        deps = [],
        visibility = None,
        testonly = 0):  # pylint: disable=unused-argument
    py_proto_library(
        name = name,
        srcs = srcs,
        srcs_version = "PY2AND3",
        deps = ["@protobuf_archive//:protobuf_python"] + deps,
        default_runtime = "@protobuf_archive//:protobuf_python",
        protoc = "@protobuf_archive//:protoc",
        visibility = visibility,
        testonly = testonly,
    )

def _serving_copts():
    return ["-Wall", "-Wextra", "-Wpedantic", "-Wconversion"]

def serving_cc_library(**kwargs):
    kwargs["copts"] = kwargs.get("copts", []) + _serving_copts()
    cc_library(**kwargs)

def serving_cc_binary(**kwargs):
    kwargs["copts"] = kwargs.get("copts", []) + _serving_copts()
    cc_binary(**kwargs)

def serving_cc_test(**kwargs):
    kwargs["copts"] = kwargs.get("copts", []) + _serving_copts()
    cc_test(**kwargs)
