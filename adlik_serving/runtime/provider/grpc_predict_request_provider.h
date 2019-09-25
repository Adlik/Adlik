#ifndef ADLIK_SERVING_RUNTIME_PROVIDER_GRPC_PREDICT_REQUEST_PROVIDER_H
#define ADLIK_SERVING_RUNTIME_PROVIDER_GRPC_PREDICT_REQUEST_PROVIDER_H

#include <memory>

#include "adlik_serving/runtime/provider/predict_request_provider.h"
#include "tensorflow/core/lib/core/status.h"

namespace adlik {
namespace serving {

struct PredictRequest;

struct GRPCPredictRequestProvider : public PredictRequestProvider {
  static tensorflow::Status create(const ModelId& id,
                                   const PredictRequest& request,
                                   std::unique_ptr<GRPCPredictRequestProvider>* provider);

  size_t batchSize() const;
  size_t inputSize() const;
  void visitInputs(InputVisitor) const;
  void outputNames(OutputNames&) const;

private:
  GRPCPredictRequestProvider(const ModelId& id, const PredictRequest& req);

  const PredictRequest& req;
};

}  // namespace serving
}  // namespace adlik

#endif /* RUNTIME_PROVIDER_MODEL_GRPC_PREDICT_REQUEST_PROVIDER_H_ */
