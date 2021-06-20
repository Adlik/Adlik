load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "tvm_model",
    srcs = [
        "tvm_model.cc",
    ],
    hdrs = [
        "tvm_model.h",
    ],
    linkopts = [
        "-L/usr/local/lib",
        "-ltvm_runtime",
    ],
    deps = [
        "//adlik_serving/framework/domain:dims_list",
        "//adlik_serving/framework/domain:model_config_helper",
        "//adlik_serving/framework/domain:model_config_proto",
        "//adlik_serving/runtime/batching:batching_model",
        "//adlik_serving/runtime/batching:composite_batch_processor",
        "//adlik_serving/runtime/provider:predict_request_provider",
        "//adlik_serving/runtime/provider:predict_response_provider",
        "//cub/base:assertions",
        "//cub/env/fs:path",
        "//cub/log",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)

serving_cc_library(
    name = "tvm_runtime",
    srcs = ["tvm_runtime.cc"],
    deps = [
        ":tvm_model",
        "//adlik_serving/runtime/util:unique_batcher_runtime_helper",
    ],
    alwayslink = 1,
)
