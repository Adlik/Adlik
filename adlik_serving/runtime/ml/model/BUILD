load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "ml_model",
    srcs = [
        "ml_model.cc",
    ],
    hdrs = [
        "ml_model.h",
    ],
    deps = [
        "//adlik_serving/apis:task_proto",
        "//adlik_serving/framework/domain",
        "//adlik_serving/runtime/ml/algorithm",
        "//adlik_serving/runtime/provider:predict_request_provider",
        "//adlik_serving/runtime/provider:predict_response_provider",
        "//cub/base:status_wrapper",
        "//cub/env/concurrent:auto_lock",
        "//cub/env/posix",
        "//cub/log",
        "//cub/task:simple_executor",
    ],
)

serving_cc_library(
    name = "ml_runtime",
    srcs = ["ml_runtime.cc"],
    deps = [
        ":ml_model",
        "//adlik_serving/framework/domain",
        "//adlik_serving/framework/manager",
        "//adlik_serving/runtime/util:default_model_factory",
        "//adlik_serving/runtime/util:default_model_wrapper",
    ],
    alwayslink = 1,
)
