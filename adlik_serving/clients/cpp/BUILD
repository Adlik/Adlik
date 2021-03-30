load("//adlik_serving:serving.bzl", "serving_cc_binary")

serving_cc_binary(
    name = "cpp_client",
    srcs = [
        "cpp_client.cc",
    ],
    deps = [
        "//adlik_serving/apis:predict_service_proto",
        "@com_github_grpc_grpc//:grpc++",
    ],
)
