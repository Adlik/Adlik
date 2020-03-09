#ifndef ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSOR_UTILITIES_H
#define ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSOR_UTILITIES_H

#include "tensorflow/core/framework/tensor.pb.h"
#include "tensorflow/core/framework/types.pb.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/lite/c/c_api_internal.h"

namespace adlik {
namespace serving {
tensorflow::DataType tfLiteTypeToTfType(TfLiteType tfLiteType);

tensorflow::Status copyTensorProtoToTfLiteTensor(const tensorflow::TensorProto& tensorProto,
                                                 TfLiteTensor& tfLiteTensor);
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_RUNTIME_TENSORFLOW_LITE_TENSOR_UTILITIES_H
