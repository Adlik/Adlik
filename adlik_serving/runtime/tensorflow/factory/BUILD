package(
    default_visibility = [
        "//adlik_serving:internal",
    ],
)

load("//adlik_serving:serving.bzl", "serving_cc_library", "serving_proto_library")

serving_proto_library(
    name = "tf_plan_model_config_proto",
    srcs = ["tf_plan_model_config.proto"],
    cc_api_version = 2,
    deps = [
        "@org_tensorflow//tensorflow/core:protos_all_cc",
        "@protobuf_archive//:cc_wkt_protos",
    ],
)

serving_cc_library(
    name = "tf_plan_model_config",
    srcs = ["tf_plan_model_config.cc"],
    hdrs = ["tf_plan_model_config.h"],
    deps = [
        ":tf_plan_model_config_proto",
        ":tf_plan_model_options",
        "//adlik_serving/framework/domain",
        "//adlik_serving/runtime/tensorflow/model",
        "@org_tensorflow//tensorflow/cc/saved_model:loader",
        "@org_tensorflow//tensorflow/cc/saved_model:tag_constants",
        "@org_tensorflow//tensorflow/contrib/session_bundle:bundle_shim",
        "@org_tensorflow//tensorflow/core:core_cpu",
        "@org_tensorflow//tensorflow/core:lib",
        "@org_tensorflow//tensorflow/core:protos_all_cc",
    ],
)

serving_cc_library(
    name = "tf_plan_model_options",
    srcs = ["tf_plan_model_options.cc"],
    hdrs = ["tf_plan_model_options.h"],
    deps = [
        ":tf_plan_model_config_proto",
        "//cub/base:fwddecl",
        "//cub/cli:program_options",
        "//cub/protobuf:text_protobuf",
        "//cub/string:str_utils",
    ],
)

serving_cc_library(
    name = "tf_model_factory",
    srcs = ["tf_model_factory.cc"],
    hdrs = ["tf_model_factory.h"],
    deps = [
        "//adlik_serving/runtime/tensorflow/model:plan_model_loader",
    ],
)

serving_cc_library(
    name = "tf_runtime",
    srcs = ["tf_runtime.cc"],
    hdrs = ["tf_runtime.h"],
    deps = [
        ":tf_model_factory",
        ":tf_plan_model_config",
        "//adlik_serving/apis:predict_service_proto",
        "//adlik_serving/framework/manager",
        "//adlik_serving/runtime/tensorflow/model",
        "//cub/base:optional",
        "//cub/dci",
        "//cub/mem:read_write_ptr",
    ],
    alwayslink = 1,
)
