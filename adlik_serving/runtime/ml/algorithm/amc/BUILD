load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "amc",
    srcs = [
        "amc.cc",
    ],
    deps = [
        "//adlik_serving/framework/domain:algorithm_config_proto",
        "//adlik_serving/runtime/ml/algorithm:algorithm_factory",
        "//adlik_serving/runtime/ml/algorithm/proto:task_config_proto",
        "//cub/log",
    ],
    alwayslink = 1,
)
