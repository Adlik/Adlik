# -*- Python -*-
"""
Adlik serving dependencies, this file should be called before workspace.bzl
"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

def add_all_deps():
    """All Adlik serving external dependencies."""

    # Python rules.

    http_archive(
        name = "rules_python",
        sha256 = "64a3c26f95db470c32ad86c924b23a821cd16c3879eed732a7841779a32a60f8",
        strip_prefix = "rules_python-748aa53d7701e71101dfd15d800e100f6ff8e5d1",
        urls = ["https://github.com/bazelbuild/rules_python/archive/748aa53d7701e71101dfd15d800e100f6ff8e5d1.tar.gz"],
    )

    # C++ rules.

    http_archive(
        name = "rules_cc",
        sha256 = "e637dc045e70e43b7431fbeacb68d569a84f9fe1cb5879c432aa0855097dce57",
        strip_prefix = "rules_cc-34ca16f4aa4bf2a5d3e4747229202d6cb630ebab",
        urls = ["https://github.com/bazelbuild/rules_cc/archive/34ca16f4aa4bf2a5d3e4747229202d6cb630ebab.tar.gz"],
    )

    # Bazel extra actions proto file.

    http_file(
        name = "bazel_extra_actions_base_proto",
        sha256 = "0d91fe29a80d2754f13e379713da37750c42425d92ef7de77f60acdb82479edc",
        urls = ["https://github.com/bazelbuild/bazel/raw/3.4.1/src/main/protobuf/extra_actions_base.proto"],
    )

    # TensorFlow

    http_archive(
        name = "org_tensorflow",
        sha256 = "9c94bfec7214853750c7cacebd079348046f246ec0174d01cd36eda375117628",
        strip_prefix = "tensorflow-582c8d236cb079023657287c318ff26adb239002",
        urls = [
            "https://github.com/tensorflow/tensorflow/archive/582c8d236cb079023657287c318ff26adb239002.tar.gz",
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