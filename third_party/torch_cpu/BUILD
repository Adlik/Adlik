load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "torch_cpu",
    srcs = [
        "lib/libc10.so",
        "lib/libcaffe2_detectron_ops.so",
        "lib/libcaffe2_module_test_dynamic.so",
        "lib/libcaffe2_observers.so",
        "lib/libgomp-75eea7e8.so.1",
        "lib/libjitbackend_test.so",
        "lib/libprocess_group_agent.so",
        "lib/libshm.so",
        "lib/libtensorpipe_agent.so",
        "lib/libtorch.so",
        "lib/libtorch_cpu.so",
        "lib/libtorch_global_deps.so",
        "lib/libtorchbind_test.so",
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
    ],
    visibility = ["//visibility:public"],
)
