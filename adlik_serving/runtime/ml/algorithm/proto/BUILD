load("//adlik_serving:serving.bzl", "serving_proto_library", "serving_proto_library_py")

package(default_visibility = ["//adlik_serving:internal"])

serving_proto_library(
    name = "amc_task_proto",
    srcs = ["amc_task.proto"],
    cc_api_version = 2,
)

serving_proto_library_py(
    name = "amc_task_proto_py_pb2",
    srcs = ["amc_task.proto"],
    proto_library = "amc_task_proto",
)

serving_proto_library(
    name = "grid_task_proto",
    srcs = ["grid_task.proto"],
    cc_api_version = 2,
)

serving_proto_library_py(
    name = "grid_task_proto_py_pb2",
    srcs = ["grid_task.proto"],
    proto_library = "grid_task_proto",
)

serving_proto_library(
    name = "task_config_proto",
    srcs = ["task_config.proto"],
    cc_api_version = 2,
    deps = [
        ":amc_task_proto",
        ":grid_task_proto",
    ],
)

serving_proto_library_py(
    name = "task_config_proto_py_pb2",
    srcs = ["task_config.proto"],
    proto_library = "task_config_proto",
    deps = [
        ":amc_task_proto_py_pb2",
        ":grid_task_proto_py_pb2",
    ],
)
