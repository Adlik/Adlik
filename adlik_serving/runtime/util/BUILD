load("//adlik_serving:serving.bzl", "serving_cc_library")

package(
    default_visibility = [
        "//adlik_serving:internal",
    ],
    features = ["-layering_check"],
)

serving_cc_library(
    name = "name_def",
    hdrs = [
        "name_def.h",
    ],
)

serving_cc_library(
    name = "queue_options",
    hdrs = [
        "queue_options.h",
    ],
    deps = [
        "//adlik_serving/framework/domain:model_config_proto",
    ],
)

serving_cc_library(
    name = "default_model_wrapper",
    hdrs = [
        "default_model_wrapper.h",
    ],
    deps = [
        "//adlik_serving/framework/domain",
        "//adlik_serving/framework/manager",
    ],
)

serving_cc_library(
    name = "default_model_factory",
    hdrs = [
        "default_model_factory.h",
    ],
    deps = [
        "//adlik_serving/framework/domain",
        "//adlik_serving/framework/manager",
    ],
)

serving_cc_library(
    name = "default_runtime_helper",
    hdrs = [
        "default_runtime_helper.h",
    ],
    deps = [
        ":default_model_factory",
        ":default_model_wrapper",
        ":name_def",
        "//adlik_serving/framework/domain",
        "//adlik_serving/framework/manager",
        "//adlik_serving/runtime/provider:predict_util",
    ],
)

serving_cc_library(
    name = "shared_batcher_wrapper",
    srcs = [
        "shared_batcher_wrapper.cc",
    ],
    hdrs = [
        "shared_batcher_wrapper.h",
    ],
    deps = [
        "//adlik_serving/runtime/batching:batch_scheduler",
        "//adlik_serving/runtime/batching:batching_message_task",
        "//adlik_serving/runtime/batching:shared_batch_scheduler",
        "//cub/dci",
        "//cub/log",
    ],
)

serving_cc_library(
    name = "shared_batcher_model_wrapper",
    hdrs = [
        "shared_batcher_model_wrapper.h",
    ],
    deps = [
        "//adlik_serving/framework/domain",
        "//adlik_serving/framework/manager",
        "//adlik_serving/runtime/batching:batch_scheduler",
        "//adlik_serving/runtime/util:shared_batcher_wrapper",
        "//cub/base:status",
    ],
)

serving_cc_library(
    name = "shared_batcher_model_factory",
    hdrs = [
        "shared_batcher_model_factory.h",
    ],
    deps = [
        "//adlik_serving/framework/manager",
        "//cub/base:status",
    ],
)

serving_cc_library(
    name = "shared_batcher_runtime_helper",
    hdrs = [
        "shared_batcher_runtime_helper.h",
    ],
    deps = [
        ":name_def",
        ":shared_batcher_model_factory",
        ":shared_batcher_model_wrapper",
        ":shared_batcher_wrapper",
        "//adlik_serving/framework/domain",
        "//adlik_serving/framework/manager",
        "//adlik_serving/runtime/batching:batching_message_task",
        "//adlik_serving/runtime/batching:shared_batch_scheduler",
        "//adlik_serving/runtime/provider:predict_util",
        "//cub/base:assertions",
    ],
)

serving_cc_library(
    name = "unique_batcher_model_wrapper",
    hdrs = [
        "unique_batcher_model_wrapper.h",
    ],
    deps = [
        ":queue_options",
        "//adlik_serving/framework/domain",
        "//adlik_serving/framework/manager",
        "//adlik_serving/runtime/batching:basic_batch_scheduler",
        "//adlik_serving/runtime/batching:batch_scheduler",
        "//adlik_serving/runtime/batching:batching_message_task",
        "//cub/base:status",
    ],
)

serving_cc_library(
    name = "unique_batcher_runtime_helper",
    hdrs = ["unique_batcher_runtime_helper.h"],
    deps = [
        ":default_runtime_helper",
        ":unique_batcher_model_wrapper",
    ],
)
