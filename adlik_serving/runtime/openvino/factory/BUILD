load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "openvino_runtime",
    srcs = ["openvino_runtime.cc"],
    linkopts = [
        "-linference_engine",
    ],
    deps = [
        "//adlik_serving/runtime/openvino/model:ir_model",
        "//adlik_serving/runtime/util:unique_batcher_runtime_helper",
        "@local_config_openvino//:libinference_engine",
        "@local_config_openvino//:libinference_engine_legacy",
    ],
    alwayslink = 1,
)
