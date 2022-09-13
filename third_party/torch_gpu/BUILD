load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "torch_cpu",
    srcs = [
        "lib/libc10.so",
        "lib/libc10_cuda.so",
        "lib/libcudart-80664282.so.10.2",
        "lib/libgomp-75eea7e8.so.1",
        "lib/libnvToolsExt-3965bdd0.so.1",
        "lib/libtorch.so",
        "lib/libtorch_cpu.so",
        "lib/libtorch_cuda.so",
    ],
    hdrs = glob(["include/**/*.h"]),
    copts = ["-D_GLIBCXX_USE_CXX11_ABI=1"],
    includes = [
        "include",
        "include/TH",
        "include/THC",
        "include/torch/csrc/api/include",
    ],
    linkopts = [
        "-ltorch",
        "-lc10",
        "-ltorch_cpu",
        "-ltorch_cuda",
        "-lc10_cuda",
    ],
    visibility = ["//visibility:public"],
)
