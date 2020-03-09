load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "predict_request_provider",
    hdrs = [
        "predict_request_provider.h",
    ],
    deps = [
        "//adlik_serving/framework/domain",
    ],
)

serving_cc_library(
    name = "predict_response_provider",
    hdrs = [
        "predict_response_provider.h",
    ],
    deps = [
        "//adlik_serving/framework/domain:dims_list",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)

serving_cc_library(
    name = "grpc_predict_request_provider",
    srcs = ["grpc_predict_request_provider.cc"],
    hdrs = [
        "grpc_predict_request_provider.h",
    ],
    deps = [
        ":predict_request_provider",
        "//adlik_serving/apis:predict_proto",
        "//cub/log",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)

serving_cc_library(
    name = "grpc_predict_response_provider",
    srcs = ["grpc_predict_response_provider.cc"],
    hdrs = [
        "grpc_predict_response_provider.h",
    ],
    deps = [
        ":predict_response_provider",
        "//adlik_serving/apis:predict_proto",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)

serving_cc_library(
    name = "predict_util",
    srcs = [
        "predict_util.cc",
    ],
    hdrs = [
        "predict_util.h",
    ],
    deps = [
        ":grpc_predict_request_provider",
        ":grpc_predict_response_provider",
        "//adlik_serving/framework/domain",
        "//cub/base:fwddecl",
    ],
)
