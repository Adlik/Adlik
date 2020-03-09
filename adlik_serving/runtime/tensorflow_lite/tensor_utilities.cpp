#include "adlik_serving/runtime/tensorflow_lite/tensor_utilities.h"

using tensorflow::DataType;
using tensorflow::Status;
using tensorflow::TensorProto;

namespace adlik {
namespace serving {
DataType tfLiteTypeToTfType(TfLiteType tfLiteType) {
  switch (tfLiteType) {
    case TfLiteType::kTfLiteNoType:
      // https://github.com/tensorflow/tensorflow/blob/4601949937145e66df37483c460ba9b7bfdfa680/tensorflow/lite/delegates/flex/util.cc#L60
      return DataType::DT_FLOAT;
    case TfLiteType::kTfLiteFloat32:
      return DataType::DT_FLOAT;
    case TfLiteType::kTfLiteInt32:
      return DataType::DT_INT32;
    case TfLiteType::kTfLiteUInt8:
      return DataType::DT_UINT8;
    case TfLiteType::kTfLiteInt64:
      return DataType::DT_INT64;
    case TfLiteType::kTfLiteString:
      return DataType::DT_STRING;
    case TfLiteType::kTfLiteBool:
      return DataType::DT_BOOL;
    case TfLiteType::kTfLiteInt16:
      return DataType::DT_INT16;
    case TfLiteType::kTfLiteComplex64:
      return DataType::DT_COMPLEX64;
    case TfLiteType::kTfLiteInt8:
      return DataType::DT_INT8;
    case TfLiteType::kTfLiteFloat16:
      return DataType::DT_HALF;
    default:
      throw std::logic_error("Unreachable");
  }
}

Status copyTensorProtoToTfLiteTensor(const TensorProto& tensorProto, TfLiteTensor& tfLiteTensor) {
  throw std::logic_error("Not implemented");
}
}  // namespace serving
}  // namespace adlik
