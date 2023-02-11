# -*- Python -*-

"""
Adlik serving dependencies
"""

load("@org_tensorflow//tensorflow:version_check.bzl", "check_bazel_version_at_least")

def adlik_serving_workspace():
    """All Adlik serving external dependencies."""

    check_bazel_version_at_least(minimum_bazel_version = "3.7.2")
