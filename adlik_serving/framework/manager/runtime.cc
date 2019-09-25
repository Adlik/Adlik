#include "adlik_serving/framework/manager/runtime.h"

#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

tensorflow::Status Runtime::predict(const RunOptions&, ModelHandle*, const PredictRequest&, PredictResponse&) {
  return tensorflow::errors::Internal("predict function not implemented!");
}

}  // namespace serving
}  // namespace adlik
