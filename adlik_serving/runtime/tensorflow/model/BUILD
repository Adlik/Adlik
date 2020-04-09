load("//adlik_serving:serving.bzl", "serving_cc_library")

package(
    default_visibility = [
        "//adlik_serving:internal",
    ],
    features = ["-layering_check"],
)

serving_cc_library(
    name = "model",
    srcs = [
        "meta_graph.cc",
        "model_signature.cc",
    ],
    hdrs = [
        "errors.h",
        "meta_graph.h",
        "model_inputs.h",
        "model_outputs.h",
        "model_signature.h",
        "plan_model.h",
    ],
    deps = [
        "//adlik_serving/framework/manager",
        "//adlik_serving/runtime/tensorflow/factory:tf_plan_model_config_proto",
        "//adlik_serving/runtime/tensorflow/factory:tf_plan_model_options",
        "//cub/base:comparator",
        "//cub/base:hash",
        "//cub/dci",
        "@org_tensorflow//tensorflow/cc/saved_model:tag_constants",
        "@org_tensorflow//tensorflow/core:framework",
    ],
)

serving_cc_library(
    name = "plan_model_loader",
    srcs = [
        "plan_model_factory.cc",
        "plan_model_loader.cc",
    ],
    hdrs = [
        "plan_model_factory.h",
        "plan_model_loader.h",
    ],
    deps = [
        ":model",
        "//adlik_serving/runtime/tensorflow/batch",
        "//cub/env/fs:path",
    ],
)
