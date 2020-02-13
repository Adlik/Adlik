load("//adlik_serving:serving.bzl", "serving_cc_library", "serving_proto_library", "serving_proto_library_py")
load(
    "@org_tensorflow//tensorflow:tensorflow.bzl",
    "tf_cuda_library",
)

package(
    default_visibility = [
        "//adlik_serving:internal",
    ],
)

serving_proto_library(
    name = "version_policy_proto",
    srcs = ["version_policy.proto"],
    cc_api_version = 2,
    deps = [
        "@com_google_protobuf//:cc_wkt_protos",
    ],
)

serving_proto_library_py(
    name = "version_policy_proto_py_pb2",
    srcs = ["version_policy.proto"],
    proto_library = "version_policy_proto",
    deps = [],
)

# "@org_tensorflow//tensorflow/core:protos_all_py",

serving_proto_library(
    name = "model_config_proto",
    srcs = ["model_config.proto"],
    cc_api_version = 2,
    deps = [
        ":version_policy_proto",
        "@org_tensorflow//tensorflow/core:protos_all",
    ],
)

serving_proto_library_py(
    name = "model_config_proto_py_pb2",
    srcs = ["model_config.proto"],
    proto_library = "model_config_proto",
    deps = [
        "version_policy_proto_py_pb2",
        "@org_tensorflow//tensorflow/core:protos_all_py",
    ],
)

serving_proto_library(
    name = "model_spec_proto",
    srcs = ["model_spec.proto"],
    cc_api_version = 2,
    deps = [
        "@com_google_protobuf//:cc_wkt_protos",
    ],
)

serving_proto_library_py(
    name = "model_spec_proto_py_pb2",
    srcs = ["model_spec.proto"],
    proto_library = "model_spec_proto",
)

serving_cc_library(
    name = "model_options",
    srcs = ["model_options.cc"],
    hdrs = ["model_options.h"],
    deps = ["//cub/cli:program_options"],
)

serving_cc_library(
    name = "dims_list",
    hdrs = ["dims_list.h"],
    deps = [
    ],
)

tf_cuda_library(
    name = "model_config_helper",
    srcs = ["model_config_helper.cc"],
    hdrs = ["model_config_helper.h"],
    deps = [
        ":dims_list",
        ":model_config_proto",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)

serving_cc_library(
    name = "domain",
    srcs = [
        "aspired_state.cc",
        "event_bus.cc",
        "label_provider.cc",
        "model_config.cc",
        "model_id.cc",
        "model_loader.cc",
        "model_path.cc",
        "model_request.cc",
        "model_state.cc",
        "model_store.cc",
        "model_stream.cc",
        "monitored_event.cc",
        "monitored_model.cc",
        "monitored_probe.cc",
        "monitored_store.cc",
        "shared_model_handle.cc",
        "state_monitor.cc",
        "version_list.cc",
    ],
    hdrs = [
        "aspired_state.h",
        "auto_model_handle.h",
        "bare_model_loader.h",
        "event_bus.h",
        "event_handler.h",
        "label_provider.h",
        "model.h",
        "model_config.h",
        "model_handle.h",
        "model_id.h",
        "model_loader.h",
        "model_path.h",
        "model_request.h",
        "model_source.h",
        "model_state.h",
        "model_store.h",
        "model_stream.h",
        "model_target.h",
        "monitored_event.h",
        "monitored_model.h",
        "monitored_notifier.h",
        "monitored_predicate.h",
        "monitored_probe.h",
        "monitored_store.h",
        "shared_model.h",
        "shared_model_handle.h",
        "state_monitor.h",
        "version_list.h",
    ],
    deps = [
        ":model_config_helper",
        ":model_config_proto",
        ":model_options",
        ":model_spec_proto",
        "//cub/base:assertions",
        "//cub/base:comparator",
        "//cub/base:hash",
        "//cub/base:optional",
        "//cub/dci",
        "//cub/env/concurrent:auto_lock",
        "//cub/env/concurrent:mutex",
        "//cub/env/concurrent:notification",
        "//cub/env/fs",
        "//cub/env/fs:path",
        "//cub/env/time",
        "//cub/log",
        "//cub/mem:any_ptr",
        "//cub/protobuf:text_protobuf",
        "//cub/string:string_view",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)
