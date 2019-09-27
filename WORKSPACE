workspace(name = "adlik")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

# Python rules.

http_archive(
    name = "rules_python",
    sha256 = "43c007823228f88d6afe1580d00f349564c97e103309a234fa20a5a10a9ff85b",
    strip_prefix = "rules_python-54d1cb35cd54318d59bf38e52df3e628c07d4bbc",
    urls = ["https://github.com/bazelbuild/rules_python/archive/54d1cb35cd54318d59bf38e52df3e628c07d4bbc.tar.gz"],
)

http_archive(
    name = "rules_cc",
    sha256 = "7c574de35acdbfd333eb3f8eb05990b377ef8dc2303664f4ddd6cc83bbf4a30a",
    strip_prefix = "rules_cc-42ed56d8acbd9938b4ee8b2066d2c4c898a22504",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/42ed56d8acbd9938b4ee8b2066d2c4c898a22504.tar.gz"],
)

# TensorFlow.

http_archive(
    name = "org_tensorflow",
    sha256 = "c4da79385dfbfb30c1aaf73fae236bc6e208c3171851dfbe0e1facf7ca127a6a",
    strip_prefix = "tensorflow-87989f69597d6b2d60de8f112e1e3cea23be7298",
    urls = [
        "https://mirror.bazel.build/github.com/tensorflow/tensorflow/archive/87989f69597d6b2d60de8f112e1e3cea23be7298.tar.gz",
        "https://github.com/tensorflow/tensorflow/archive/87989f69597d6b2d60de8f112e1e3cea23be7298.tar.gz",
    ],
)

http_archive(
    name = "io_bazel_rules_closure",
    sha256 = "ddce3b3a3909f99b28b25071c40b7fec7e2e1d1d1a4b2e933f3082aa99517105",
    strip_prefix = "rules_closure-316e6133888bfc39fb860a4f1a31cfcbae485aef",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_closure/archive/316e6133888bfc39fb860a4f1a31cfcbae485aef.tar.gz",
        "https://github.com/bazelbuild/rules_closure/archive/316e6133888bfc39fb860a4f1a31cfcbae485aef.tar.gz",
    ],
)

http_archive(
    name = "bazel_skylib",
    sha256 = "2c62d8cd4ab1e65c08647eb4afe38f51591f43f7f0885e7769832fa137633dcb",
    strip_prefix = "bazel-skylib-0.7.0",
    urls = ["https://github.com/bazelbuild/bazel-skylib/archive/0.7.0.tar.gz"],
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

# TensorRT.

new_local_repository(
    name = "extern_lib",
    build_file_content = """
cc_library(
    name = "libnvinfer",
    srcs = ["libnvinfer.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "libnvparsers",
    srcs = ["libnvparsers.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "libnvinfer_plugin",
    srcs = ["libnvinfer_plugin.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "libnvonnxparser",
    srcs = ["libnvonnxparser.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "libcudnn",
    srcs = ["libcudnn.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "libcublas",
    srcs = ["libcublas.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "libcudart_static",
    srcs = ["libcudart_static.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "libnvToolsExt",
    srcs = ["libnvToolsExt.so"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "libcudart",
    srcs = ["libcudart.so"],
    visibility = ["//visibility:public"],
)
""",
    path = "/usr/lib/x86_64-linux-gnu",
)

# Bazel extra actions proto file.

http_file(
    name = "bazel_extra_actions_base_proto",
    sha256 = "0d91fe29a80d2754f13e379713da37750c42425d92ef7de77f60acdb82479edc",
    urls = ["https://github.com/bazelbuild/bazel/raw/0.29.1/src/main/protobuf/extra_actions_base.proto"],
)

# Adlik serving dependencies.

load("//adlik_serving:workspace.bzl", "adlik_serving_workspace")

adlik_serving_workspace()

# Specify the minimum required bazel version.

load("@org_tensorflow//tensorflow:version_check.bzl", "check_bazel_version_at_least")

check_bazel_version_at_least(minimum_bazel_version = "0.24.1")
