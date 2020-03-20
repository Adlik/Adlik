load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "amc",
    srcs = [
        "amc.cc",
    ],
    deps = [
        "//adlik_serving/apis:amc_task_proto",
        "//adlik_serving/runtime/ml/algorithm:algorithm_factory",
        "//adlik_serving/runtime/ml/algorithm/proto:amc_proto",
        "//cub/env/fs",
        "//cub/env/fs:path",
        "//cub/log",
        "//cub/protobuf:text_protobuf",
    ],
    alwayslink = 1,
)
