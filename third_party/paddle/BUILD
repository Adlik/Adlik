load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

config_setting(
    name="paddle-link-static",
    values={"define": "paddle_link_static=true"},
)

#TODO fix license label
licenses(["notice"])  # BSD

#paddle_inference
cc_import(
    name = "paddle_inference-static_lib",
    static_library = "paddle/lib/libpaddle_inference.a",
    #linkstatic = 1,
    visibility = ["//visibility:private"],
)

cc_import(
    name = "paddle_inference-shared_lib",
    shared_library = "paddle/lib/libpaddle_inference.so",
    visibility = ["//visibility:private"],
)

cc_library(
    name = "paddle_inference-lib",
    hdrs = glob(["paddle/include/**"]),
    includes = ["paddle/include"],
    deps = select({
        ":paddle-link-static": [":paddle_inference-static_lib"],
        "//conditions:default": [":paddle_inference-shared_lib"],
    }),    
)

#cryptopp
cc_library(
    name = "paddle_third_party_cryptopp",
    srcs = [
        "third_party/install/cryptopp/lib/libcryptopp.a",
    ],
    hdrs = glob(["third_party/install/cryptopp/include/**"]),
    includes = ["third_party/install/cryptopp/include"],
)

#gflags
cc_library(
    name = "paddle_third_party_gflags",
    srcs = [
        "third_party/install/gflags/lib/libgflags.a",
    ],
    hdrs = [
        "third_party/install/gflags/include/gflags/gflags.h",
        "third_party/install/gflags/include/gflags/gflags_completions.h",
        "third_party/install/gflags/include/gflags/gflags_declare.h",
        "third_party/install/gflags/include/gflags/gflags_gflags.h",
    ],
    includes = ["third_party/install/gflags/include/"],
)

#glog
cc_library(
    name = "paddle_third_party_glog",
    srcs = [
        "third_party/install/glog/lib/libglog.a",
    ],
    hdrs = glob(["third_party/install/glog/include/**"]),
    includes = ["third_party/install/glog/include"],
)

#xxhash
cc_library(
    name = "paddle_third_party_xxhash",
    srcs = [
        "third_party/install/xxhash/lib/libxxhash.a",
    ],
    hdrs = glob(["third_party/install/xxhash/include/**"]),
    includes = ["third_party/install/xxhash/include"],
)

#mkldnn
cc_import(
    name = "paddle_third_party_mkldnn_so",
    shared_library = "third_party/install/mkldnn/lib/libdnnl.so.2",
    visibility = ["//visibility:private"],
)

cc_library(
    name = "paddle_third_party_mkldnn",
    hdrs = glob(["third_party/install/mkldnn/include/**"]),
    includes = ["third_party/install/mkldnn/include"],
    deps = [
        ":paddle_third_party_mkldnn_so",
    ],
)

#mklml_intel
cc_import(
    name = "paddle_third_party_mklml_libiomp5.so",
    shared_library = "third_party/install/mklml/lib/libiomp5.so",
    visibility = ["//visibility:private"],
)

cc_import(
    name = "paddle_third_party_mklml_libmklml_intel.so",
    shared_library = "third_party/install/mklml/lib/libmklml_intel.so",
    visibility = ["//visibility:private"],
)

cc_library(
    name = "paddle_third_party_mklml_intel",
    hdrs = glob(["third_party/install/mklml/include/**"]),
    includes = ["third_party/install/mklml/include"],
    deps = [
        ":paddle_third_party_mklml_libmklml_intel.so",
        ":paddle_third_party_mklml_libiomp5.so",
    ],
)


cc_library(
    name = "paddle_inference",
    linkopts = ["-lrt -ldl -lpthread"],
    deps = [
        ":paddle_inference-lib",
        ":paddle_third_party_cryptopp",
        ":paddle_third_party_gflags",
        ":paddle_third_party_glog",
        ":paddle_third_party_xxhash",
        ":paddle_third_party_mklml_intel",
        ":paddle_third_party_mkldnn",
        "@com_google_protobuf//:protobuf",
    ],
)