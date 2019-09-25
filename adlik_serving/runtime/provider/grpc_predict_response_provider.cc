#include "adlik_serving/runtime/provider/grpc_predict_response_provider.h"

#include "adlik_serving/apis/predict.pb.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

tensorflow::Status GRPCPredictResponseProvider::create(const std::vector<std::string>& output_names,
                                                       size_t batch_size,
                                                       PredictResponse& response,
                                                       std::unique_ptr<GRPCPredictResponseProvider>* rsp_provider) {
  GRPCPredictResponseProvider* provider = new GRPCPredictResponseProvider(output_names, batch_size, response);
  rsp_provider->reset(provider);
  return tensorflow::Status::OK();
}

GRPCPredictResponseProvider::GRPCPredictResponseProvider(const std::vector<std::string>& output_names,
                                                         size_t batch_size,
                                                         PredictResponse& response)
    : PredictResponseProvider(), output_names(output_names), rsp(response), batch_size(batch_size) {
}

tensorflow::Status GRPCPredictResponseProvider::addOutput(const std::string& name,
                                                          tensorflow::DataType dtype,
                                                          const DimsList& dims,
                                                          void** buffer,
                                                          size_t buffer_byte_size) {
  for (auto& i : output_names) {
    if (i == name) {
      auto& proto = *((*rsp.mutable_outputs())[name].mutable_tensor());
      proto.set_dtype(dtype);
      auto shape_proto = proto.mutable_tensor_shape();
      shape_proto->add_dim()->set_size(batch_size);
      for (auto i = 0; i < dims.size(); ++i) {
        shape_proto->add_dim()->set_size(dims[i]);
      }
      std::string* content = proto.mutable_tensor_content();
      content->resize(buffer_byte_size);
      *buffer = static_cast<void*>(&((*content)[0]));
      return tensorflow::Status::OK();
    }
  }
  *buffer = nullptr;
  return tensorflow::Status::OK();
}

}  // namespace serving
}  // namespace adlik
