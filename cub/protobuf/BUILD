load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "text_protobuf",
    srcs = ["text_protobuf.cc"],
    hdrs = ["text_protobuf.h"],
    deps = [
        "//cub/base:assertions",
        "//cub/env",
        "//cub/env/fs",
        "@com_google_protobuf//:protobuf_headers",
    ],
)
