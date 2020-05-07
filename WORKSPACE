workspace(name = "adlik")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

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

# TensorFlow.

http_archive(
    name = "org_tensorflow",
    sha256 = "b3d7829fac84e3a26264d84057367730b6b85b495a0fce15929568f4b55dc144",
    strip_prefix = "tensorflow-2b96f3662bd776e277f86997659e61046b56c315",
    urls = [
        "https://mirror.bazel.build/github.com/tensorflow/tensorflow/archive/2b96f3662bd776e277f86997659e61046b56c315.tar.gz",
        "https://github.com/tensorflow/tensorflow/archive/2b96f3662bd776e277f86997659e61046b56c315.tar.gz",
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
    urls = ["https://github.com/bazelbuild/bazel/raw/3.1.0/src/main/protobuf/extra_actions_base.proto"],
)

# Adlik serving dependencies.

load("//adlik_serving:workspace.bzl", "adlik_serving_workspace")

adlik_serving_workspace()

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@upb//bazel:repository_defs.bzl", "bazel_version_repository")

bazel_version_repository(name = "bazel_version")

# Specify the minimum required bazel version.

load("@org_tensorflow//tensorflow:version_check.bzl", "check_bazel_version_at_least")

check_bazel_version_at_least(minimum_bazel_version = "2.1.0")
