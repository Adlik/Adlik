cc_library(
    name = "torchvision",
    srcs = [
        "lib/libtorchvision.so",
 
    ],
    linkopts = [
        "-ltorchvision",
    ],
    hdrs = glob(["include/**/*.h"]),
    includes = [
        "include",
    ],
    #deps=[
     #   "@libtorch_archive//:torch_cpu",
    #],

    visibility = ["//visibility:public"]
)