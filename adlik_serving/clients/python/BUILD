sh_binary(
    name = "build_pip_package",
    srcs = ["build_pip_package.sh"],
    data = [
        "setup.py",
        "//adlik_serving/apis:predict_service_proto_py_pb2",
        "//adlik_serving/apis:model_operate_service_proto_py_pb2",
        "//adlik_serving/framework/domain:model_config_proto_py_pb2",
        "//adlik_serving/framework/domain:model_spec_proto_py_pb2",
        "//adlik_serving/framework/domain:version_policy_proto_py_pb2",
    ],
)
