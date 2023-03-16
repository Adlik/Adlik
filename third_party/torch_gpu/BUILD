cc_library(
    name = "torch_gpu",
    srcs = [
        "lib/libtorch_cpu.so",
        "lib/libtorch.so",
        "lib/libc10.so",
        "lib/libc10_cuda.so",
        "lib/libtorch_cuda.so",
        "lib/libcudart-45da57e3.so.11.0",
        "lib/libgomp-52f2fd74.so.1",
        "lib/libnvToolsExt-847d78f2.so.1",
    ],
    hdrs = glob(["include/**/*.h"]),
    copts = ["-D_GLIBCXX_USE_CXX11_ABI=1"],
    includes = [
        "include",
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
    visibility = ["//visibility:public"]
)
