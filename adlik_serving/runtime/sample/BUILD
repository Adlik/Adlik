package(default_visibility = ["//adlik_serving:internal"])

load("//adlik_serving:serving.bzl", "serving_cc_library")

serving_cc_library(
    name = "no_scheduler_runtime",
    srcs = ["no_scheduler_runtime.cc"],
    deps = [
        "//adlik_serving/runtime/provider:predict_request_provider",
        "//adlik_serving/runtime/provider:predict_response_provider",
        "//adlik_serving/runtime/util:default_runtime_helper",
    ],
    alwayslink = 1,
)

serving_cc_library(
    name = "shared_scheduler_runtime",
    srcs = ["shared_scheduler_runtime.cc"],
    deps = [
        "//adlik_serving/runtime/batching:batch_processor",
        "//adlik_serving/runtime/batching:batching_model",
        "//adlik_serving/runtime/util:shared_batcher_runtime_helper",
    ],
    alwayslink = 1,
)

serving_cc_library(
    name = "unique_scheduler_runtime",
    srcs = ["unique_scheduler_runtime.cc"],
    deps = [
        "//adlik_serving/runtime/batching:batching_model",
        "//adlik_serving/runtime/batching:composite_batch_processor",
        "//adlik_serving/runtime/util:unique_batcher_runtime_helper",
    ],
    alwayslink = 1,
)

serving_cc_library(
    name = "sample",
    deps = [
        ":no_scheduler_runtime",
        ":shared_scheduler_runtime",
        ":unique_scheduler_runtime",
    ],
)
