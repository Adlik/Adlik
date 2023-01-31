load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "torch_cpu",
    srcs = [
        "lib/libbackend_with_compiler.so",
        "lib/libc10.so",
        "lib/libgomp-52f2fd74.so.1",
        "lib/libjitbackend_test.so",
        "lib/libnnapi_backend.so",
        "lib/libshm.so",
        "lib/libtorch.so",
        "lib/libtorch_cpu.so",
        "lib/libtorch_global_deps.so",
        "lib/libtorchbind_test.so",
    ],
    hdrs = glob(["include/**/*.h"]),
    copts = ["-D_GLIBCXX_USE_CXX11_ABI=1"],
    includes = [
        "include",
        "include/torch/csrc/api/include",
    ],
    linkopts = [
        "-ltorch",
        "-lc10",
        "-ltorch_cpu",
    ],
    visibility = ["//visibility:public"],
)
