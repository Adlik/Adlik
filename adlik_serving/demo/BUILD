load("//adlik_serving:serving.bzl", "serving_cc_binary")

serving_cc_binary(
    name = "demo",
    srcs = [
        "decode_png.cc",
        "decode_png.h",
        "image.h",
        "main.cc",
        "predict_service.cc",
        "predict_service.h",
    ],
    deps = [
        "//adlik_serving/apis:get_model_meta_impl",
        "//adlik_serving/apis:predict_impl",
        "@png",
    ] + select({
        "//adlik_serving/server/core:tensorflow": ["//adlik_serving/runtime/tensorflow/factory:tf_runtime"],
        "//adlik_serving/server/core:tensorflow-lite": ["//adlik_serving/runtime/tensorflow_lite:tensorflow_lite_runtime"],
        "//adlik_serving/server/core:tensorrt": ["//adlik_serving/runtime/tensorrt/factory:trt_runtime"],
        "//adlik_serving/server/core:openvino": ["//adlik_serving/runtime/openvino/factory:openvino_runtime"],
        "//adlik_serving/server/core:ml": ["//adlik_serving/runtime/ml/model:ml_runtime"],
        "//adlik_serving/server/core:topsinference": ["//adlik_serving/runtime/topsinference/factory:topsinference_runtime"],
    }),
)
