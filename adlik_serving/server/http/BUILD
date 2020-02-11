load("//adlik_serving:serving.bzl", "serving_cc_library")

package(
    default_visibility = [
        "//adlik_serving:internal",
    ],
)

serving_cc_library(
    name = "http_options",
    srcs = ["http_options.cc"],
    hdrs = ["http_options.h"],
    deps = [
        "//adlik_serving/server/http/internal:request_handler_options",
        "//adlik_serving/server/http/internal:server_interface",
        "//cub/cli:option_subscriber",
        "//cub/cli:program_options",
        "//cub/dci",
        "//cub/log",
    ],
)

serving_cc_library(
    name = "http_rest_api_handler",
    srcs = ["http_rest_api_handler.cc"],
    hdrs = ["http_rest_api_handler.h"],
    deps = [
        "//adlik_serving/apis:get_model_meta_impl",
        "//adlik_serving/apis:predict_impl",
        "//adlik_serving/framework/manager",
        "//adlik_serving/server/http/internal:request_handler_options",
        "//cub/dci",
        "//cub/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)

serving_cc_library(
    name = "http",
    srcs = ["http_server.cc"],
    hdrs = [
        "http_server.h",
    ],
    deps = [
        ":http_options",
        ":http_rest_api_handler",
        "//adlik_serving/server/http/internal:http_status_code",
        "//adlik_serving/server/http/internal:server_interface",
        "//cub/base:status",
        "//cub/dci",
        "//cub/log",
        "@com_googlesource_code_re2//:re2",
    ],
)
