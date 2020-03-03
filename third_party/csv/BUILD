load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "csv",
    hdrs = [
        "include/csv/concurrent_queue.hpp",
        "include/csv/dialect.hpp",
        "include/csv/reader.hpp",
        "include/csv/robin_hood.hpp",
        "include/csv/writer.hpp",
    ],
    copts = ["-std=c++17"],
    includes = ["include"],
)
