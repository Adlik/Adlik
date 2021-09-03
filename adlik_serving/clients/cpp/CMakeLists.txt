cmake_minimum_required(VERSION 3.16)

project(cc_client)

set(CMAKE_CXX_STANDARD 14)

include(./cmake/common.cmake)

include_directories(adlik_serving/apis/)
include_directories(adlik_serving/framework/domain/)
include_directories(tensorflow/core/framework/)

add_executable(cc_client
        adlik_serving/apis/get_model_meta.grpc.pb.cc
        adlik_serving/apis/get_model_meta.grpc.pb.h
        adlik_serving/apis/get_model_meta.pb.cc
        adlik_serving/apis/get_model_meta.pb.h
        adlik_serving/apis/predict.grpc.pb.cc
        adlik_serving/apis/predict.grpc.pb.h
        adlik_serving/apis/predict.pb.cc
        adlik_serving/apis/predict.pb.h
        adlik_serving/apis/predict_service.grpc.pb.cc
        adlik_serving/apis/predict_service.grpc.pb.h
        adlik_serving/apis/predict_service.pb.cc
        adlik_serving/apis/predict_service.pb.h
        adlik_serving/framework/domain/model_config.grpc.pb.cc
        adlik_serving/framework/domain/model_config.grpc.pb.h
        adlik_serving/framework/domain/model_config.pb.cc
        adlik_serving/framework/domain/model_config.pb.h
        adlik_serving/framework/domain/model_spec.grpc.pb.cc
        adlik_serving/framework/domain/model_spec.grpc.pb.h
        adlik_serving/framework/domain/model_spec.pb.cc
        adlik_serving/framework/domain/model_spec.pb.h
        adlik_serving/framework/domain/version_policy.grpc.pb.cc
        adlik_serving/framework/domain/version_policy.grpc.pb.h
        adlik_serving/framework/domain/version_policy.pb.cc
        adlik_serving/framework/domain/version_policy.pb.h
        tensorflow/core/framework/resource_handle.grpc.pb.cc
        tensorflow/core/framework/resource_handle.grpc.pb.h
        tensorflow/core/framework/resource_handle.pb.cc
        tensorflow/core/framework/resource_handle.pb.h
        tensorflow/core/framework/tensor.grpc.pb.cc
        tensorflow/core/framework/tensor.grpc.pb.h
        tensorflow/core/framework/tensor.pb.cc
        tensorflow/core/framework/tensor.pb.h
        tensorflow/core/framework/tensor_shape.grpc.pb.cc
        tensorflow/core/framework/tensor_shape.grpc.pb.h
        tensorflow/core/framework/tensor_shape.pb.cc
        tensorflow/core/framework/tensor_shape.pb.h
        tensorflow/core/framework/types.grpc.pb.cc
        tensorflow/core/framework/types.grpc.pb.h
        tensorflow/core/framework/types.pb.cc
        tensorflow/core/framework/types.pb.h
        cpp_client.cc)

target_link_libraries(cc_client
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})
