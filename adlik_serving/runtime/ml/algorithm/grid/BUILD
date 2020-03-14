load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "grid",
    srcs = [
        "grid_impl.cc",
        "grid_input.cc",
    ],
    hdrs = [
        "grid_input.h",
        "grid_output.h",
        "kmeans.h",
        "types.h",
    ],
    deps = [
        "//adlik_serving/runtime/ml/algorithm:algorithm_factory",
        "//adlik_serving/runtime/ml/algorithm/proto:task_config_proto",
        "//csv",
        "//cub/base:status_wrapper",
        "//cub/env/fs",
        "//cub/env/fs:path",
        "//cub/log",
        "//cub/string:string_view",
        "@dlib_archive//:dlib",
    ],
    alwayslink = 1,
)
