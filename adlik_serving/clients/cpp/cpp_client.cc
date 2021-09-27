// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

// The example for visit grpc DL model predict

#include <iostream>
#include <vector>

#include "adlik_serving/apis/predict_service.grpc.pb.h"
#include "grpcpp/grpcpp.h"

using adlik::serving::GetModelMetaRequest;
using adlik::serving::GetModelMetaResponse;
using adlik::serving::ModelInput;
using adlik::serving::PredictRequest;
using adlik::serving::PredictRequest_Output;
using adlik::serving::PredictResponse;
using adlik::serving::PredictService;
using google::protobuf::int32;
using google::protobuf::MapPair;
using google::protobuf::RepeatedField;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using tensorflow::DataType;
using tensorflow::TensorProto;

TensorProto make_tensor_proto(const RepeatedField<int64_t>& dims,
                              const int32 maxBatchSize,
                              const tensorflow::DataType dataType) {
  TensorProto result;

  result.set_dtype(dataType);

  result.mutable_tensor_shape()->add_dim()->set_size(maxBatchSize);
  auto numPixels = maxBatchSize;
  for (auto dim_i : dims) {
    result.mutable_tensor_shape()->add_dim()->set_size(dim_i);
    numPixels *= dim_i;
  }
  // here should be your true input data
  std::vector<float> fakeData(numPixels, 0.5);
  for (auto it = fakeData.begin(); it != fakeData.end(); ++it) {
    union {
      float source;
      char target[sizeof(float)];
    } converter;

    converter.source = *it;
    std::copy(
        std::begin(converter.target), std::end(converter.target), std::back_inserter(*result.mutable_tensor_content()));
  }
  return result;
}

PredictRequest make_predict_request(const GetModelMetaResponse& getModelMetaResponse) {
  PredictRequest request;
  auto config = getModelMetaResponse.config();
  request.mutable_model_spec()->set_name(config.name());
  request.mutable_model_spec()->set_signature_name("predict");
  request.set_batch_size(config.max_batch_size());
  for (auto model_input : config.input()) {
    request.mutable_inputs()->insert(MapPair<std::string, TensorProto>{
        model_input.name(), make_tensor_proto(model_input.dims(), config.max_batch_size(), model_input.data_type())});
  }
  for (auto model_output : config.output()) {
    request.mutable_output_filter()->insert(
        MapPair<std::string, PredictRequest_Output>{model_output.name(), PredictRequest_Output()});
  }
  return request;
}

int main(int argc, char* argv[]) {
  // create grpc stub
  const std::string grpcUrl = "localhost:8500";
  std::shared_ptr<Channel> channel = grpc::CreateChannel(grpcUrl, grpc::InsecureChannelCredentials());
  std::unique_ptr<PredictService::Stub> predictServiceStub(PredictService::NewStub(channel));
  // get model config
  ClientContext getModelContext;
  GetModelMetaRequest getModelMetaRequest;
  getModelMetaRequest.mutable_model_spec()->set_name("mnist");
  getModelMetaRequest.mutable_model_spec()->set_signature_name("predict");
  GetModelMetaResponse getModelMetaResponse;
  Status getModelStatus =
      predictServiceStub->getModelMeta(&getModelContext, getModelMetaRequest, &getModelMetaResponse);
  if (getModelStatus.ok()) {
    std::cout << "model name: " << getModelMetaResponse.model_spec().name() << std::endl;
    std::cout << "max batch size: " << getModelMetaResponse.config().max_batch_size() << std::endl;
  } else {
    std::cout << getModelStatus.error_code() << ": " << getModelStatus.error_message() << std::endl;
  }
  // get predict result
  ClientContext predictContext;
  PredictRequest predictRequest = make_predict_request(getModelMetaResponse);
  PredictResponse predictResponse;
  Status predictStatus = predictServiceStub->predict(&predictContext, predictRequest, &predictResponse);
  if (predictStatus.ok()) {
    std::cout << "result:" << std::endl;
    for (auto output : predictResponse.outputs()) {
      auto outputTensor = output.second.tensor();
      const void* content = outputTensor.tensor_content().c_str();
      size_t contentSize = outputTensor.tensor_content().size();
      float* f_data = (float*)content;
      std::cout << "output name: " << output.first << std::endl << ", value: " << std::endl;
      for (size_t i = 0; i < contentSize / sizeof(float); i++) {
        std::cout << *(f_data + i) << ", ";
      }
      std::cout << std::endl;
    }
  } else {
    std::cout << predictStatus.error_code() << ": " << predictStatus.error_message() << std::endl;
  }
}
