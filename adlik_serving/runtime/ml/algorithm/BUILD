load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility = ["//visibility:public"])

serving_cc_library(
    name = "kmeans",
    hdrs = [
        "kmeans.h",
    ],
    deps = [
        "@dlib_archive//:dlib",
    ],
)

serving_cc_library(
    name = "algorithm_factory",
    srcs = [
        "algorithm_factory.cc",
        "algorithm_register.cc",
    ],
    hdrs = [
        "algorithm.h",
        "algorithm_creator.h",
        "algorithm_factory.h",
        "algorithm_register.h",
    ],
    deps = [
        "//cub/base:status_wrapper",
        "//cub/gof:singleton",
        "//cub/log",
    ],
)

serving_cc_library(
    name = "algorithm",
    linkstatic = 1,
    deps = [
        ":algorithm_factory",
    ],
    alwayslink = 1,
)
