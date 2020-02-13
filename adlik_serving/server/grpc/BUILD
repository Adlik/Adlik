load("//adlik_serving:serving.bzl", "serving_cc_library", "serving_proto_library")

package(
    default_visibility = [
        "//adlik_serving:internal",
    ],
)

serving_proto_library(
    name = "ssl_config_proto",
    srcs = ["ssl_config.proto"],
    cc_api_version = 2,
    deps = [
        "@com_google_protobuf//:cc_wkt_protos",
    ],
)

serving_cc_library(
    name = "grpc_options",
    srcs = ["grpc_options.cc"],
    hdrs = ["grpc_options.h"],
    deps = [
        ":ssl_config_proto",
        "//cub/base:fwddecl",
        "//cub/cli:option_subscriber",
        "//cub/cli:program_options",
        "//cub/protobuf:text_protobuf",
        "@grpc//:grpc++",
    ],
)

serving_cc_library(
    name = "grpc_service",
    srcs = ["grpc_service.cc"],
    hdrs = ["grpc_service.h"],
    deps = [
        "//adlik_serving/apis:get_model_meta_impl",
        "//adlik_serving/apis:predict_impl",
        "//adlik_serving/apis:predict_service_proto",
        "//cub/dci",
        "@grpc//:grpc++",
    ],
)

serving_cc_library(
    name = "grpc",
    srcs = ["grpc_server.cc"],
    hdrs = ["grpc_server.h"],
    deps = [
        ":grpc_options",
        ":grpc_service",
        "//cub/base:assertions",
        "@grpc//:grpc++",
    ],
)
