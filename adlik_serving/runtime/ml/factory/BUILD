load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "ml_runtime",
    srcs = ["ml_runtime.cc"],
    deps = [
        "//adlik_serving/framework/domain",
        "//adlik_serving/framework/manager",
        "//adlik_serving/runtime/ml/model:ml_model",
        "//adlik_serving/runtime/util:default_model_factory",
        "//adlik_serving/runtime/util:default_model_wrapper",
    ],
    alwayslink = 1,
)
