load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "trt_runtime",
    srcs = ["trt_runtime.cc"],
    linkopts = [
        "-pthread",
        "-lnvinfer",
        "-lnvidia-ml",
        "-L/usr/local/cuda/lib64/stubs",
    ],
    deps = [
        "//adlik_serving/runtime/tensorrt/model:trt_model",
        "//adlik_serving/runtime/util:unique_batcher_runtime_helper",
    ],
    alwayslink = 1,
)
