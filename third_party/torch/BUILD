cc_library(
    name = "torch_cpu",
    srcs = [
        "lib/libtorch.so",
        "lib/libc10.so",
        "lib/libtorch_cpu.so",
        "lib/libc10_cuda.so",
        "lib/libtorch_cuda.so",
        "lib/libcudart-80664282.so.10.2",
        "lib/libgomp-75eea7e8.so.1",
        "lib/libnvToolsExt-3965bdd0.so.1"
    ],
    linkopts = [
        "-ltorch",
        "-lc10",
        "-ltorch_cpu",
        "-ltorch_cuda",
        "-lc10_cuda",
    ],
    hdrs = glob(["include/**/*.h"]),
    includes = [
        "include",
        "include/torch/csrc/api/include",
        "include/THC",
        "include/TH",
    ],
    copts = ["-D_GLIBCXX_USE_CXX11_ABI=1"],
    visibility = ["//visibility:public"]
)

