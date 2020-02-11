load("//adlik_serving:serving.bzl", "serving_cc_binary")

package(
    default_visibility = ["//adlik_serving:internal"],
)

package_group(
    name = "internal",
    packages = [
        "//adlik_serving/...",
    ],
)

serving_cc_binary(
    name = "adlik_serving",
    srcs = ["server/main.cc"],
    deps = [
        "//adlik_serving/server/core:server_core",
        "//adlik_serving/server/core:version",
    ],
)
