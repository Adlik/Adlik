load("//adlik_serving:serving.bzl", "serving_cc_library")
load("@rules_cc//cc:defs.bzl", "cc_test")

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
        "grid_impl.cc",
        "grid_input.cc",
        # "k-means.cc",  # should remove
    ],
    hdrs = [
        "grid_input.h",
        "grid_output.h",
        "kmeans.h",
        "types.h",
    ],
    deps = [
        ":csv_reader",
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

cc_test(
    name = "csv_reader_test",
    srcs = ["csv_reader_test.cc"],
    deps = [
        ":csv_reader",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)
