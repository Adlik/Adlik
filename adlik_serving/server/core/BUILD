load("//adlik_serving:serving.bzl", "serving_cc_library")

package(
    default_visibility = [
        "//adlik_serving:internal",
    ],
)

serving_cc_library(
    name = "version",
    srcs = ["version.cc"],
    hdrs = ["version.h"],
    deps = [
        "//cub/base:symbol",
    ],
)

config_setting(
    name = "tensorflow",
    values = {"define": "runtime=tensorflow"},
)

config_setting(
    name = "tensorrt",
    values = {"define": "runtime=tensorrt"},
)

config_setting(
    name = "openvino",
    values = {"define": "runtime=openvino"},
)

config_setting(
    name = "ml",
    values = {"define": "runtime=ml"},
)

serving_cc_library(
    name = "server_core",
    srcs = [
        "server_core.cc",
    ],
    hdrs = [
        "server_core.h",
    ],
    deps = [
        "//cub/cli:program_options",
        "//adlik_serving/framework/manager",
        "//adlik_serving/server/grpc",
        "//adlik_serving/server/http",
        "//adlik_serving/apis:predict_impl",
        "//adlik_serving/apis:get_model_meta_impl",
        "//adlik_serving/apis:task_op_impl",
    ] + select({
        ":tensorflow": ["//adlik_serving/runtime/tensorflow/factory:tf_runtime"],
        ":tensorrt": ["//adlik_serving/runtime/tensorrt/factory:trt_runtime"],
        ":openvino": ["//adlik_serving/runtime/openvino/factory:openvino_runtime"],
        ":ml": ["//adlik_serving/runtime/ml/factory:ml_runtime"],
    }),
)
