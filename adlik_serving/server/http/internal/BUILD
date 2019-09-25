package(
    default_visibility = [
        "//adlik_serving:internal",
    ],
)

load("//adlik_serving:serving.bzl", "serving_cc_library")

# interface
serving_cc_library(
    name = "event_executor",
    srcs = [
        "event_executor_impl.cc",
    ],
    hdrs = [
        "event_executor.h",
    ],
    deps = [
        "//cub/task:concurrent_executor",
    ],
)

serving_cc_library(
    name = "event_loop_scheduler",
    hdrs = [
        "event_loop_scheduler.h",
    ],
)

serving_cc_library(
    name = "request_handler_options",
    hdrs = ["request_handler_options.h"],
)

serving_cc_library(
    name = "http_status_code",
    hdrs = [
        "http_status_code.h",
    ],
)

serving_cc_library(
    name = "operation_tracker",
    srcs = [
        "operation_tracker.cc",
    ],
    hdrs = ["operation_tracker.h"],
    deps = [
        "@com_google_absl//absl/synchronization",
    ],
)

# impl
serving_cc_library(
    name = "http_request",
    srcs = [
        "http_request.cc",
    ],
    hdrs = ["http_request.h"],
    deps = [
        ":event_loop_scheduler",
        ":http_status_code",
        ":operation_tracker",
        ":request_handler_options",
        "//cub/base:uncloneable",
        "//cub/log",
        "@com_github_libevent_libevent//:libevent",
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/types:span",
    ],
)

serving_cc_library(
    name = "server_interface",
    srcs = [
        "server_interface.cc",
    ],
    hdrs = ["server_interface.h"],
    deps = [
        ":event_executor",
        ":http_request",
        ":http_status_code",
        ":operation_tracker",
        ":request_handler_options",
    ],
)
