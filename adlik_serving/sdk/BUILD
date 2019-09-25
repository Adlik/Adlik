package(
    default_visibility = ["//adlik_serving:internal"],
)

load("//adlik_serving:serving.bzl", "serving_cc_library")

serving_cc_library(
    name = "sdk_api",
    hdrs = ["sdk_api.h"],
    # srcs = ["sdk_api.cc"],
    deps = [
        "//adlik_serving/server/core:server_core",
    ],
)
