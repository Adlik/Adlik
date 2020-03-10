load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "grid",
    srcs = [
        "k-means.cc",
    ],
    hdrs = [
    ],
    # copts = ["-std=c++17"],
    # linkstatic = 1,
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

# cc_binary(
#     name = "k-means-test",
#     srcs = ["k-means-test.cc"],
#     copts = ["-std=c++1z"],
#     linkopts = ["-lpthread"],
#     deps = [
#         ":algorithm",
#         "@xunit_cut//:cut",
#         "@xunit_cut//:cut_main",
#     ],
# )
