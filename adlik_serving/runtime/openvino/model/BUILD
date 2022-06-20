load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "ir_model",
    srcs = [
        "ir_model.cc",
        "openvino_util.cc",
    ],
    hdrs = [
        "ir_model.h",
        "openvino_util.h",
    ],
    deps = [
        "//adlik_serving/framework/domain:dims_list",
        "//adlik_serving/framework/domain:model_config_helper",
        "//adlik_serving/framework/domain:model_config_proto",
        "//adlik_serving/runtime/batching:batching_model",
        "//adlik_serving/runtime/batching:composite_batch_processor",
        "//adlik_serving/runtime/openvino:ns",
        "//adlik_serving/runtime/provider:predict_request_provider",
        "//adlik_serving/runtime/provider:predict_response_provider",
        "//cub/log",
        "@local_config_openvino//:ie_headers",
        "@local_config_openvino//:libopenvino",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)
