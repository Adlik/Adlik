load("@rules_cc//cc:defs.bzl", "cc_binary")
load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//adlik_serving:internal"])

serving_cc_library(
    name = "algorithm",
    srcs = [
        "algorithm_factory.cc",
        "algorithm_register.cc",
        "k-means.cc",
    ],
    hdrs = [
        "algorithm.h",
        "algorithm_config.h",
        "algorithm_creator.h",
        "algorithm_factory.h",
        "algorithm_register.h",
        "ml_task.h",
    ],
    copts = ["-std=c++17"],
    linkstatic = 1,
    deps = [
        "//cub/base:status_wrapper",
        "//cub/env/fs",
        "//cub/env/fs:path",
        "//cub/gof:singleton",
        "//cub/log",
        "//cub/string:string_view",
        "@csv",
        "@dlib_archive//:dlib",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "k-means-test",
    srcs = ["k-means-test.cc"],
    copts = ["-std=c++17"],
    linkopts = ["-lpthread"],
    deps = [
        ":algorithm",
        "@xunit_cut//:cut",
        "@xunit_cut//:cut_main",
    ],
)
