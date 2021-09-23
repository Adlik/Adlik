load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "paddle_model",
    srcs = [
        "paddle_model.cc",
        "paddle_util.cc",
    ],
    hdrs = [
        "paddle_model.h",
        "paddle_util.h",
    ],
    deps = [
        "//adlik_serving/framework/domain:dims_list",
        "//adlik_serving/framework/domain:model_config_helper",
        "//adlik_serving/framework/domain:model_config_proto",
        "//adlik_serving/runtime/batching:batching_model",
        "//adlik_serving/runtime/batching:composite_batch_processor",
        "//adlik_serving/runtime/provider:predict_request_provider",
        "//adlik_serving/runtime/provider:predict_response_provider",
        "//adlik_serving/runtime/util:datatype_size",
        "//cub/log",
        "@paddle_inference",
    ],
)
