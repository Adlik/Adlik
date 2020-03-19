load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "csv_reader",
    srcs = [
        "csv_reader.cc",
    ],
    hdrs = [
        "csv_reader.h",
    ],
    deps = [
        "//cub/log",
        "//cub/string:str_utils",
    ],
)

serving_cc_library(
    name = "grid",
    srcs = [
        "cell.cc",
        "grid_csv_saver.cc",
        "grid_impl.cc",
        "grid_input.cc",
        "grid_output.cc",
        "neighbors.cc",
        "rsrp_grid.cc",
    ],
    hdrs = [
        "cell.h",
        "grid_csv_saver.h",
        "grid_input.h",
        "grid_output.h",
        "kmeans.h",
        "neighbors.h",
        "rsrp_grid.h",
        "types.h",
    ],
    deps = [
        ":csv_reader",
        "//adlik_serving/framework/domain:algorithm_config_proto",
        "//adlik_serving/framework/manager:time_stats",
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
