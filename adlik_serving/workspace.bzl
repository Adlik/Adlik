# -*- Python -*-

"""
Adlik serving dependencies
"""

load("@org_tensorflow//tensorflow:workspace.bzl", "tf_workspace")
load("//third_party/openvino:openvino_configure.bzl", "openvino_configure")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def adlik_serving_workspace():
    """All Adlik serving external dependencies."""

    openvino_configure(name = "local_config_openvino")

    tf_workspace(path_prefix = "", tf_repo_name = "org_tensorflow")

    http_archive(
        name = "com_github_libevent_libevent",
        urls = [
            "https://github.com/libevent/libevent/archive/release-2.1.8-stable.zip",
        ],
        sha256 = "70158101eab7ed44fd9cc34e7f247b3cae91a8e4490745d9d6eb7edc184e4d96",
        strip_prefix = "libevent-release-2.1.8-stable",
        build_file = "@//third_party/libevent:BUILD",
    )
