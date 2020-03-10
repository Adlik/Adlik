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
