load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "cuda",
    hdrs = glob([
        "include/CL/*.h*",
        "include/Openacc/*.h*",
        "include/Openmp/*.h*",
        "include/cooperative_groups/*.h*",
        "include/crt/*.h*",
        "include/cooperative_groups/*.h*",
        "include/*.h*",
    ]),
    visibility = ["//visibility:public"],
)
