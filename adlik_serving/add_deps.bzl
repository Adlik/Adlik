# -*- Python -*-
"""
Adlik serving dependencies, this file should be called before workspace.bzl
"""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

def add_vision(name):
    native.new_local_repository(
        name = name,
        path = "/vision-0.9.1",
        build_file = "//third_party/torchvision:BUILD",
    )

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
        sha256 = "329e25bf4a344de9cc70508b96019d19bffce338c69c7b12f60a84ee23448a1b",
        strip_prefix = "tensorflow-319f094d0b23520e2631c900e66e0c28baf84e74",
        urls = [
            "https://github.com/tensorflow/tensorflow/archive/319f094d0b23520e2631c900e66e0c28baf84e74.tar.gz",
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
            "https://paddle-inference-library-base-product.oss-cn-beijing.aliyuncs.com/paddle-inference-library-2.4.0.zip",
        ],
        sha256 = "0282e9d2fff851f3a5665fbff5a24d7f7ffa274cb27e4091eb1ebcce682b4d3f",
        strip_prefix = "paddle-inference-library",
        build_file = str(Label("//third_party/paddle:BUILD")),
    )

    # This archive is added to fix the following issue: https://github.com/bazelbuild/bazel/issues/13811
    # When upgrade TensorFlow to v2.8.0, try to delete this.
    http_archive(
        name = "build_bazel_rules_apple",
        sha256 = "0052d452af7742c8f3a4e0929763388a66403de363775db7e90adecb2ba4944b",
        urls = [
            "https://github.com/bazelbuild/rules_apple/releases/download/0.31.3/rules_apple.0.31.3.tar.gz",
        ],
    )

    http_archive(
        name = "libtorch_gpu_archive",
        strip_prefix = "libtorch",
        sha256 = "5a392132fbff9db1482eae72a30f74b09f53a47edf8305fe9688d4ce7ddb0b6b",
        type = "zip",
        urls = ["https://download.pytorch.org/libtorch/cu116/libtorch-cxx11-abi-shared-with-deps-1.12.1%2Bcu116.zip"],
        build_file = str(Label("//third_party/torch_gpu:BUILD")),
    )

    http_archive(
        name = "libtorch_cpu_archive",
        strip_prefix = "libtorch",
        sha256 = "44ecf7f2fc671da7b67694fc9d64b2762039ad83de46407ae3e0868081b7ae2c",
        type = "zip",
        urls = ["https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.8.1%2Bcpu.zip"],
        build_file = str(Label("//third_party/torch_cpu:BUILD")),
    )
