load("//adlik_serving:serving.bzl", "serving_cc_library")

package(
    default_visibility = [
        "//adlik_serving:internal",
    ],
)

serving_cc_library(
    name = "time_stats",
    srcs = ["time_stats.cc"],
    hdrs = ["time_stats.h"],
    deps = [
        "//cub/log",
    ],
)

serving_cc_library(
    name = "run_options",
    hdrs = ["run_options.h"],
)

serving_cc_library(
    name = "manager",
    srcs = [
        "boarding_functor.cc",
        "boarding_loop.cc",
        "managed_store.cc",
        "model_factory_suite.cc",
        "runtime.cc",
        "runtime_suite.cc",
        "serving_store.cc",
        "storage_loop.cc",
    ],
    hdrs = [
        "auto_runtime_register.h",
        "boarding_functor.h",
        "boarding_loop.h",
        "managed_store.h",
        "model_factory.h",
        "model_factory_suite.h",
        "model_map.h",
        "runtime.h",
        "runtime_context.h",
        "runtime_suite.h",
        "serving_store.h",
        "storage_loop.h",
    ],
    deps = [
        ":run_options",
        "//adlik_serving/framework/domain",
        "//cub/base:fwddecl",
        "//cub/base:optional",
        "//cub/base:status_wrapper",
        "//cub/base:symbol",
        "//cub/dci",
        "//cub/env/concurrent",
        "//cub/env/concurrent:loop_thread",
        "//cub/gof:singleton",
        "//cub/log",
        "//cub/mem:read_write_ptr",
        "//cub/string:str_utils",
        "@org_tensorflow//tensorflow/core:lib",
    ],
)
