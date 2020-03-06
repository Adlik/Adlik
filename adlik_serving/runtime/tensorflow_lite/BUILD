load("//adlik_serving:serving.bzl", "serving_cc_library")

serving_cc_library(
    name = "tensorflow_lite_runtime",
    srcs = [
        "tensorflow_lite_batch_processor.cc",
        "tensorflow_lite_model.cc",
        "tensorflow_lite_runtime.cc",
    ],
    hdrs = [
        "tensor_shape_dims.h",
        "tensorflow_lite_batch_processor.h",
        "tensorflow_lite_model.h",
    ],
    visibility = ["//adlik_serving:internal"],
    deps = [
        "//adlik_serving/runtime/batching:batching_model",
        "//adlik_serving/runtime/batching:composite_batch_processor",
        "//adlik_serving/runtime/util:unique_batcher_runtime_helper",
        "@com_google_absl//absl/hash",
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    ],
    alwayslink = 1,
)
