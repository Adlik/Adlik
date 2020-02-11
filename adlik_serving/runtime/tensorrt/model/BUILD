load(
    "@org_tensorflow//tensorflow:tensorflow.bzl",
    "tf_cuda_library",
)

package(default_visibility = ["//adlik_serving:internal"])

tf_cuda_library(
    name = "trt_util",
    srcs = ["trt_util.cc"],
    hdrs = ["trt_util.h"],
    deps = [
        "//adlik_serving/framework/domain:dims_list",
        "//adlik_serving/framework/domain:model_config_proto",
        "@local_config_cuda//cuda:cudart",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)

tf_cuda_library(
    name = "trt_instance",
    srcs = [
        "trt_instance.cc",
    ],
    hdrs = [
        "trt_instance.h",
    ],
    deps = [
        ":trt_util",
        "//adlik_serving/framework/manager:time_stats",
        "//adlik_serving/runtime/batching:batch_processor",
        "//adlik_serving/runtime/batching:batch_scheduler",
        "//adlik_serving/runtime/batching:batching_message_task",
        "//adlik_serving/runtime/provider:predict_request_provider",
        "//adlik_serving/runtime/provider:predict_response_provider",
        "//cub/base:fwddecl",
        "//cub/log",
    ],
)

tf_cuda_library(
    name = "trt_model",
    srcs = [
        "trt_model.cc",
    ],
    hdrs = [
        "trt_model.h",
    ],
    deps = [
        ":trt_instance",
        ":trt_util",
        "//adlik_serving/framework/domain:model_config_helper",
        "//adlik_serving/runtime/batching:basic_batch_scheduler",
        "//adlik_serving/runtime/batching:batching_model",
        "//adlik_serving/runtime/batching:composite_batch_processor",
        "//cub/env/posix",
        "//cub/log",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)
