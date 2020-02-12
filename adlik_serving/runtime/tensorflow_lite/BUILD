load("//adlik_serving:serving.bzl", "serving_cc_library")

serving_cc_library(
    name = "tensorflow_lite_runtime",
    srcs = ["tensorflow_lite_runtime.cc"],
    visibility = ["//adlik_serving:internal"],
    deps = [
        "//adlik_serving/runtime/batching:batching_model",
        "//adlik_serving/runtime/batching:composite_batch_processor",
        "//adlik_serving/runtime/util:unique_batcher_runtime_helper",
    ],
    alwayslink = 1,
)
