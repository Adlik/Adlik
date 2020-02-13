workspace(name = "adlik")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

# Python rules.

http_archive(
    name = "rules_python",
    sha256 = "43c007823228f88d6afe1580d00f349564c97e103309a234fa20a5a10a9ff85b",
    strip_prefix = "rules_python-54d1cb35cd54318d59bf38e52df3e628c07d4bbc",
    urls = ["https://github.com/bazelbuild/rules_python/archive/54d1cb35cd54318d59bf38e52df3e628c07d4bbc.tar.gz"],
)

# C++ rules.

http_archive(
    name = "rules_cc",
    sha256 = "7c574de35acdbfd333eb3f8eb05990b377ef8dc2303664f4ddd6cc83bbf4a30a",
    strip_prefix = "rules_cc-42ed56d8acbd9938b4ee8b2066d2c4c898a22504",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/42ed56d8acbd9938b4ee8b2066d2c4c898a22504.tar.gz"],
)

# TensorFlow.

http_archive(
    name = "tf_serving",
    sha256 = "7e499926ac29491bd69d0a31ba834c12eba67e4b79576a16f34a67137a6af05b",
    strip_prefix = "serving-d83512c6b5b2b8433df2fd61bbbfb22e0295b3d3",
    urls = [
        "https://github.com/tensorflow/serving/archive/d83512c6b5b2b8433df2fd61bbbfb22e0295b3d3.tar.gz",
    ],
)

http_archive(
    name = "org_tensorflow",
    sha256 = "1f4b09e6bff7f847bb1034699076055e50e87534d76008af8295ed71195b2b36",
    strip_prefix = "tensorflow-e5bf8de410005de06a7ff5393fafdf832ef1d4ad",
    urls = [
        "https://mirror.bazel.build/github.com/tensorflow/tensorflow/archive/e5bf8de410005de06a7ff5393fafdf832ef1d4ad.tar.gz",
        "https://github.com/tensorflow/tensorflow/archive/e5bf8de410005de06a7ff5393fafdf832ef1d4ad.tar.gz",
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

http_archive(
    name = "bazel_skylib",
    sha256 = "1dde365491125a3db70731e25658dfdd3bc5dbdfd11b840b3e987ecf043c7ca0",
    urls = ["https://github.com/bazelbuild/bazel-skylib/releases/download/0.9.0/bazel_skylib-0.9.0.tar.gz"],
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
    urls = ["https://github.com/bazelbuild/bazel/raw/2.1.0/src/main/protobuf/extra_actions_base.proto"],
)

# Adlik serving dependencies.

load("//adlik_serving:workspace.bzl", "adlik_serving_workspace")

adlik_serving_workspace()

# Specify the minimum required bazel version.

load("@org_tensorflow//tensorflow:version_check.bzl", "check_bazel_version_at_least")

check_bazel_version_at_least(minimum_bazel_version = "2.1.0")
