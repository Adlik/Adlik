load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "torchvision",
    srcs = [
        "lib/libtorchvision.so",
    ],
    hdrs = glob(["include/**/*.h"]),
    includes = [
        "include",
    ],
    linkopts = [
        "-ltorchvision",
    ],
    visibility = ["//visibility:public"],
)
