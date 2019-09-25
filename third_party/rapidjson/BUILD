load("@rules_cc//cc:defs.bzl", "cc_library")

# RapidJSON (rapidjson.org) library.
# from https://github.com/Tencent/rapidjson

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "rapidjson",
    hdrs = glob(["include/rapidjson/**/*.h"]),
    includes = ["include"],
)
