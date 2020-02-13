# -*- Python -*-

"""
Adlik serving dependencies
"""

load("@tf_serving//tensorflow_serving:workspace.bzl", "tf_serving_workspace")
load("//third_party/openvino:openvino_configure.bzl", "openvino_configure")

def adlik_serving_workspace():
    """All Adlik serving external dependencies."""

    openvino_configure(name = "local_config_openvino")

    tf_serving_workspace()
