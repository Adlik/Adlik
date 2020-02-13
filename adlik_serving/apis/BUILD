load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")
load("//adlik_serving:serving.bzl", "serving_cc_library", "serving_proto_library", "serving_proto_library_py")

package(
    default_visibility = [
        "//adlik_serving:internal",
    ],
)

serving_proto_library(
    name = "predict_proto",
    srcs = ["predict.proto"],
    cc_api_version = 2,
    deps = [
        "//adlik_serving/framework/domain:model_spec_proto",
        "@org_tensorflow//tensorflow/core:protos_all",
    ],
)

serving_proto_library_py(
    name = "predict_proto_py_pb2",
    srcs = ["predict.proto"],
    proto_library = "predict_proto",
    deps = [
        "//adlik_serving/framework/domain:model_spec_proto_py_pb2",
        "@org_tensorflow//tensorflow/core:protos_all_py",
    ],
)

serving_proto_library(
    name = "get_model_meta_proto",
    srcs = ["get_model_meta.proto"],
    cc_api_version = 2,
    deps = [
        "//adlik_serving/framework/domain:model_config_proto",
        "//adlik_serving/framework/domain:model_spec_proto",
    ],
)

serving_proto_library_py(
    name = "get_model_meta_proto_py_pb2",
    srcs = ["get_model_meta.proto"],
    proto_library = "get_model_meta_proto",
    deps = [
        "//adlik_serving/framework/domain:model_config_proto_py_pb2",
        "//adlik_serving/framework/domain:model_spec_proto_py_pb2",
    ],
)

serving_proto_library(
    name = "predict_service_proto",
    srcs = ["predict_service.proto"],
    has_services = 1,
    cc_api_version = 2,
    cc_grpc_version = 1,
    deps = [
        ":get_model_meta_proto",
        ":predict_proto",
    ],
)

py_proto_library(
    name = "predict_service_proto_py_pb2",
    srcs = ["predict_service.proto"],
    default_runtime = "@com_google_protobuf//:protobuf_python",
    protoc = "@com_google_protobuf//:protoc",
    srcs_version = "PY2AND3",
    use_grpc_plugin = True,
    deps = [
        ":get_model_meta_proto_py_pb2",
        ":predict_proto_py_pb2",
        "@com_google_protobuf//:protobuf_python",
    ],
)

serving_cc_library(
    name = "predict_impl",
    srcs = ["predict_impl.cc"],
    hdrs = ["predict_impl.h"],
    deps = [
        ":predict_proto",
        "//adlik_serving/framework/manager",
        "//adlik_serving/framework/manager:time_stats",
        "//cub/dci",
    ],
)

serving_cc_library(
    name = "get_model_meta_impl",
    srcs = ["get_model_meta_impl.cc"],
    hdrs = ["get_model_meta_impl.h"],
    deps = [
        ":get_model_meta_proto",
        "//adlik_serving/framework/manager",
        "//cub/dci",
    ],
)
