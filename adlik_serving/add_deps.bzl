# -*- Python -*-
"""
Adlik serving dependencies, this file should be called before workspace.bzl
"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

def add_all_deps():
    """All Adlik serving external dependencies."""

    # Bazel extra actions proto file.

    http_file(
        name = "bazel_extra_actions_base_proto",
        sha256 = "0d91fe29a80d2754f13e379713da37750c42425d92ef7de77f60acdb82479edc",
        urls = ["https://github.com/bazelbuild/bazel/raw/3.4.1/src/main/protobuf/extra_actions_base.proto"],
    )

    # TensorFlow

    http_archive(
        name = "org_tensorflow",
        sha256 = "add5982a3ce3b9964b7122dd0d28927b6a9d9abd8f95a89eda18ca76648a0ae8",
        strip_prefix = "tensorflow-c2363d6d025981c661f8cbecf4c73ca7fbf38caf",
        urls = [
            "https://github.com/tensorflow/tensorflow/archive/c2363d6d025981c661f8cbecf4c73ca7fbf38caf.tar.gz",
        ],
    )

    http_archive(
        name = "io_bazel_rules_closure",
        sha256 = "5b00383d08dd71f28503736db0500b6fb4dda47489ff5fc6bed42557c07c6ba9",
        strip_prefix = "rules_closure-308b05b2419edb5c8ee0471b67a40403df940149",
        urls = [
            "https://storage.googleapis.com/mirror.tensorflow.org/github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",
            "https://github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",
        ],
    )

    # Xunit.

    http_archive(
        name = "xunit_cut",
        sha256 = "f7c2c339a5ab06dc1d16cb03b157a96e6c591f9833f5c072f56af4a8f8013b53",
        strip_prefix = "cut-1ce2bed5cbf7e9a282eee546ff5637a040e42abe",
        urls = [
            "https://github.com/horance-liu/cut/archive/1ce2bed5cbf7e9a282eee546ff5637a040e42abe.tar.gz",
        ],
    )

    http_archive(
        name = "com_github_libevent_libevent",
        urls = [
            "https://github.com/libevent/libevent/archive/release-2.1.8-stable.zip",
        ],
        sha256 = "70158101eab7ed44fd9cc34e7f247b3cae91a8e4490745d9d6eb7edc184e4d96",
        strip_prefix = "libevent-release-2.1.8-stable",
        build_file = str(Label("//third_party/libevent:BUILD")),
    )

    # ===== dlib dependencies, for machine learning =====
    http_archive(
        name = "dlib_archive",
        urls = [
            "https://github.com/davisking/dlib/archive/v19.19.zip",
        ],
        sha256 = "af1773d39eef0838421c70d34057e975b6d7ac885f490b6a383eb5d6da5baa84",
        strip_prefix = "dlib-19.19",
        build_file = str(Label("//third_party/dlib:BUILD")),
    )

    http_archive(
        name = "paddle_inference",
        urls = [
            "https://github.com/PaddlePaddle/Paddle/archive/refs/tags/v2.1.2.tar.gz",
        ],
        sha256 = "cd59e246958eb0bb1b19654cc1567c001888b241c69147e7503e825b1b7e0a17",
        strip_prefix = "Paddle-2.1.2",
        build_file = str(Label("//third_party/paddle:BUILD")),
    )

    http_archive(
        name = "build_bazel_rules_apple",
        sha256 = "0052d452af7742c8f3a4e0929763388a66403de363775db7e90adecb2ba4944b",
        urls = [
            "https://github.com/bazelbuild/rules_apple/releases/download/0.31.3/rules_apple.0.31.3.tar.gz",
        ],
    )
