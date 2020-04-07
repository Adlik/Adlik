load("//adlik_serving:serving.bzl", "serving_proto_library", "serving_proto_library_py")

package(default_visibility = ["//adlik_serving:internal"])

serving_proto_library(
    name = "amc_proto",
    srcs = ["amc.proto"],
    cc_api_version = 2,
    deps = [
        "@com_google_protobuf//:cc_wkt_protos",
    ],
)

serving_proto_library_py(
    name = "amc_proto_py_pb2",
    srcs = ["amc.proto"],
    proto_library = "amc_proto",
    deps = [],
)

serving_proto_library(
    name = "grid_proto",
    srcs = ["grid.proto"],
    cc_api_version = 2,
    deps = [
        "@com_google_protobuf//:cc_wkt_protos",
    ],
)

serving_proto_library_py(
    name = "grid_proto_py_pb2",
    srcs = ["grid.proto"],
    proto_library = "grid_proto",
    deps = [],
)
