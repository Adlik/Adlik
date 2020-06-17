load("//adlik_serving:serving.bzl", "serving_cc_binary")

package(
    default_visibility = ["//adlik_serving:internal"],
)

serving_cc_binary(
    name = "libadlik_serving.so",
    srcs = [
        "sdk_api.cc",
        "sdk_api.h",
    ],
    linkshared = 1,
    deps = [
        "//adlik_serving/server/core:server_core",
    ],
)
