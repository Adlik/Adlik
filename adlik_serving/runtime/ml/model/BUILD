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
        "//adlik_serving/framework/domain:model_config_helper",
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
    name = "ml_scheduler",
    srcs = [
        "scheduler.cc",
    ],
    hdrs = [
        "scheduler.h",
    ],
    deps = [
        "//adlik_serving/runtime/batching:basic_batch_scheduler",
        "//adlik_serving/runtime/batching:batch_scheduler",
        "//adlik_serving/runtime/ml/algorithm:ml_task",
    ],
)
