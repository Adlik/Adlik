load("@rules_cc//cc:defs.bzl", "cc_library")

exports_files(["LICENSE"])

cc_library(
    name = "eigen3",
    hdrs = [
        "Eigen/Core.h",
        "unsupported/Eigen/CXX11/ThreadPool.h",
    ],
    defines = [
        # This define (mostly) guarantees we don't link any problematic
        # code.
        "EIGEN_MPL2_ONLY",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@eigen_archive//:eigen",
    ],
)
