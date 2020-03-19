load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "amc",
    srcs = [
        "amc.cc",
    ],
    deps = [
        "//adlik_serving/apis:amc_task_proto",
        "//adlik_serving/framework/domain:algorithm_config_proto",
        "//adlik_serving/runtime/ml/algorithm:algorithm_factory",
        "//cub/log",
    ],
    alwayslink = 1,
)
