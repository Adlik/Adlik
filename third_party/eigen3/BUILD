load("@rules_cc//cc:defs.bzl", "cc_library")

exports_files(["LICENSE"])

cc_library(
    name = "eigen3",
    hdrs = [
        "Eigen/Core.h",
        "unsupported/Eigen/CXX11/ThreadPool.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@eigen_archive//:eigen",
    ],
)
