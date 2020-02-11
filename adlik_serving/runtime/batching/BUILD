load("//adlik_serving:serving.bzl", "serving_cc_library")

package(
    default_visibility = [
        "//adlik_serving:internal",
    ],
    features = ["-layering_check"],
)

serving_cc_library(
    name = "batch_scheduler",
    hdrs = [
        "batch_scheduler.h",
    ],
    deps = [
        "@org_tensorflow//tensorflow/core/kernels/batching_util:batch_scheduler",
    ],
)

serving_cc_library(
    name = "shared_batch_scheduler",
    hdrs = [
        "shared_batch_scheduler.h",
    ],
    deps = [
        "@org_tensorflow//tensorflow/core/kernels/batching_util:shared_batch_scheduler",
    ],
)

serving_cc_library(
    name = "basic_batch_scheduler",
    hdrs = [
        "basic_batch_scheduler.h",
    ],
    deps = [
        "@org_tensorflow//tensorflow/core/kernels/batching_util:basic_batch_scheduler",
    ],
)

serving_cc_library(
    name = "cleanup",
    hdrs = [
        "cleanup.h",
    ],
)

serving_cc_library(
    name = "batching_message_task",
    srcs = [
    ],
    hdrs = [
        "batching_message_task.h",
    ],
    deps = [
        ":batch_scheduler",
        "//cub/base:fwddecl",
    ],
)

serving_cc_library(
    name = "batch_processor",
    hdrs = [
        "batch_processor.h",
    ],
    deps = [
        ":batch_scheduler",
        ":batching_message_task",
        "//cub/base:keywords",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)

serving_cc_library(
    name = "composite_batch_processor",
    srcs = [
        "composite_batch_processor.cc",
    ],
    hdrs = [
        "composite_batch_processor.h",
    ],
    deps = [
        ":batch_processor",
        "//cub/log",
        "@org_tensorflow//tensorflow/core:framework_lite",
    ],
)

serving_cc_library(
    name = "batching_model",
    srcs = [
        "batching_model.cc",
    ],
    hdrs = [
        "batching_model.h",
    ],
    deps = [
        ":batch_processor",
        ":batch_scheduler",
        ":batching_message_task",
        ":cleanup",
        "//adlik_serving/framework/manager:run_options",
        "//adlik_serving/runtime/provider:predict_request_provider",
        "//cub/base:keywords",
        "//cub/env/time",
        "//cub/log",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)
