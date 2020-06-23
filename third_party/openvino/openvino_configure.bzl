# -*- Python -*-
"""Repository rule for Openvino configuration.

`openvino_configure` depends on the following environment variables:

  * `INTEL_CVSDK_DIR`: The intel cv sdk install path.
  * `InferenceEngine_DIR`: The library path which include intel inference engine library path.
"""

_INTEL_CVSDK_DIR = "INTEL_CVSDK_DIR"
_INFERENCE_ENGINE_DIR = "InferenceEngine_DIR"

_INFERENCE_ENGINE_LIBS = ["inference_engine"]
_INFERENCE_ENGINE_HEADERS = ["ie_plugin_dispatcher.hpp"]  # just list a few of headers for checking

def auto_configure_fail(msg):
    """Output failure message when cuda configuration fails."""
    fail("\nOpenVINO Configuration Error: %s\n" % (msg,))

def _headers_exist(repository_ctx, path):
    """Returns whether all IE header files could be found in 'path'.

    Args:
      repository_ctx: The repository context.
      path: The IE include path to check.

    Returns:
      True if all IE header files can be found in the path.
    """
    for h in _INFERENCE_ENGINE_HEADERS:
        if not repository_ctx.path("%s/%s" % (path, h)).exists:
            return False
    return True

def _find_ie_header_dir(repository_ctx, ie_path):
    """Returns the path to the directory containing headers of IE.

    Args:
      repository_ctx: The repository context.
      ie_path: The IE library install directory.

    Returns:
      The path of the directory containing the IE header.
    """
    path = str(repository_ctx.path("%s/include" % ie_path).realpath)
    if _headers_exist(repository_ctx, path):
        return path
    auto_configure_fail(
        "Cannot find headers with IE install path %s" % ie_path,
    )
    return None

def _find_ie_lib_dir(repository_ctx, ie_path):
    path = str(repository_ctx.path("%s/lib/intel64" % ie_path).realpath)
    for lib in _INFERENCE_ENGINE_LIBS:
        if not repository_ctx.path("%s/lib%s.so" % (path, lib)).exists:
            auto_configure_fail(
                "Cannot find libs dir with IE install path %s" % path,
            )
    return path

def _tpl(repository_ctx, tpl, substitutions):
    repository_ctx.template(
        tpl,
        Label("//third_party/openvino:%s.tpl" % tpl),
        substitutions,
    )

def _create_dummy_repository(repository_ctx):
    """
    Create a dummy IE repository.
    """

    _tpl(repository_ctx, "build_defs.bzl", {"%{openvino_is_configured}": "False"})
    repository_ctx.template(
        "LICENSE",
        Label("//third_party/openvino:LICENSE"),
        {},
    )
    repository_ctx.file("BUILD", """
exports_files(["LICENSE"])

package(default_visibility = ["//visibility:public"])
     """)

def _openvino_configure_impl(repository_ctx):
    """
    Implementation of the openvino_configure repository rule.
    """

    if _INTEL_CVSDK_DIR not in repository_ctx.os.environ:
        _create_dummy_repository(repository_ctx)
        return

    ie_share_dir = repository_ctx.os.environ[_INFERENCE_ENGINE_DIR].strip()
    ie_dir = repository_ctx.path("%s/.." % ie_share_dir)
    if not repository_ctx.path(ie_dir).exists:
        auto_configure_fail(
            "Cannot find inference engine install path %s." % ie_dir,
        )

    # Set up config file.
    _tpl(repository_ctx, "build_defs.bzl", {"%{openvino_is_configured}": "True"})

    ie_header_dir = _find_ie_header_dir(repository_ctx, ie_dir)
    repository_ctx.symlink(ie_header_dir, "include")
    ie_lib_dir = _find_ie_lib_dir(repository_ctx, ie_dir)
    repository_ctx.symlink(ie_lib_dir, "lib")

    repository_ctx.template(
        "LICENSE",
        Label("//third_party/openvino:LICENSE"),
        {},
    )
    repository_ctx.file("BUILD", """
exports_files(["LICENSE"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ie_headers",
    hdrs = glob(["include/**/*.h*"]),
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "libinference_engine",
    srcs = ["lib/libinference_engine.so"],
    data = ["lib/libinference_engine.so"],
    includes = [
        "include",
        "lib",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "libinference_engine_legacy",
    srcs = ["lib/libinference_engine_legacy.so"],
    data = ["lib/libinference_engine_legacy.so"],
    includes = [
        "include",
        "lib",
    ],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)
    """)

openvino_configure = repository_rule(
    implementation = _openvino_configure_impl,
    environ = [
        _INTEL_CVSDK_DIR,
        _INFERENCE_ENGINE_DIR,
    ],
)
