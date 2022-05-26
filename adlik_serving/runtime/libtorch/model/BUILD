load("//adlik_serving:serving.bzl", "serving_cc_library")

package(default_visibility=["//adlik_serving:internal"])

config_setting(
    name="resourceCuda",
    values={"define": "haveCuda=resourceCuda"},
)

config_setting(
    name="noresourceCuda",
    values={"define": "haveCuda=noresourceCuda"},
)

serving_cc_library(
    name="libtorch_model",
    srcs=[
        "libtorch_model.cc",
        "libtorch_util.cc",
    ],
    hdrs=[
        "libtorch_model.h",
        "libtorch_util.h",
    ],
    copts=["-D_GLIBCXX_USE_CXX11_ABI=1", ]+select({
        ":resourceCuda": ["-Ithird_party/cuda/include"],
        ":noresourceCuda": [],
    }),
    deps=[
        "//adlik_serving/framework/domain:dims_list",
        "//adlik_serving/framework/domain:model_config_helper",
        "//adlik_serving/framework/domain:model_config_proto",
        "//adlik_serving/runtime/batching:batching_model",
        "//adlik_serving/runtime/batching:composite_batch_processor",
        "//adlik_serving/runtime/provider:predict_request_provider",
        "//adlik_serving/runtime/provider:predict_response_provider",
        "//adlik_serving/runtime/util:datatype_size",
        "//cub/log",
        "@libtorch_archive//:torch_cpu",
        "@torchvision//:torchvision",
    ] + select({
        ":resourceCuda": ["//third_party/cuda", "@local_config_cuda//cuda:cudart", ],
        ":noresourceCuda": [],
    }),
)
