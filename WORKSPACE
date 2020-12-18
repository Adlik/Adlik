workspace(name = "adlik")

# Adlik serving dependencies.
load("//adlik_serving:add_deps.bzl", "add_all_deps")

add_all_deps()

load("//adlik_serving:workspace.bzl", "adlik_serving_workspace")

adlik_serving_workspace()

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@org_tensorflow//third_party/googleapis:repository_rules.bzl", "config_googleapis")

config_googleapis()

load("@upb//bazel:repository_defs.bzl", "bazel_version_repository")

bazel_version_repository(name = "bazel_version")
