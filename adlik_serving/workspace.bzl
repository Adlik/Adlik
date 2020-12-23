# -*- Python -*-

"""
Adlik serving dependencies
"""

load("@org_tensorflow//tensorflow:workspace.bzl", "tf_workspace")
load("@org_tensorflow//tensorflow:version_check.bzl", "check_bazel_version_at_least")
load("//third_party/openvino:openvino_configure.bzl", "openvino_configure")

def adlik_serving_workspace():
    """All Adlik serving external dependencies."""

    tf_workspace(path_prefix = "", tf_repo_name = "org_tensorflow")

    openvino_configure(name = "local_config_openvino")

    check_bazel_version_at_least(minimum_bazel_version = "3.1.0")
