licenses([
    # Note: Eigen is an MPL2 library that includes GPL v3 and LGPL v2.1+ code.
    #       We've taken special care to not reference any restricted code.
    "reciprocal",  # MPL2
    "notice",  # Portions BSD
])

exports_files(["LICENSE"])

load("@rules_cc//cc:defs.bzl", "cc_library")

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
