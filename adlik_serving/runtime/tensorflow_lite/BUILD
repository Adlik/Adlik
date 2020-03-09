load("//adlik_serving:serving.bzl", "serving_cc_library")

serving_cc_library(
    name = "tensorflow_lite_runtime_impl",
    srcs = [
        "input_context.h",
        "tensor_shape_dims.h",
        "tensor_utilities.cpp",
        "tensor_utilities.h",
        "tensorflow_lite_batch_processor.cc",
        "tensorflow_lite_batch_processor.h",
        "tensorflow_lite_engine.cc",
        "tensorflow_lite_engine.h",
        "tensorflow_lite_model.cc",
        "tensorflow_lite_model.h",
    ],
    deps = [
        "//adlik_serving/runtime/batching:batching_model",
        "//adlik_serving/runtime/batching:composite_batch_processor",
        "//adlik_serving/runtime/util:unique_batcher_runtime_helper",
        "@com_google_absl//absl/hash",
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    ],
)

serving_cc_library(
    name = "tensorflow_lite_runtime",
    srcs = [
        "tensorflow_lite_runtime.cc",
    ],
    visibility = ["//adlik_serving:internal"],
    deps = [
        ":tensorflow_lite_runtime_impl",
    ],
    alwayslink = 1,
)
