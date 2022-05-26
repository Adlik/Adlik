#include "adlik_serving/runtime/libtorch/model/libtorch_util.h"

namespace libtorch_runtime {

bool copyData2Buffer(void* content, void* dataPtr, size_t copyByteSize, size_t offsetByteSize) {
  void* bolbBuffer = dataPtr;
  bolbBuffer = (char*)bolbBuffer + offsetByteSize;
  return (content == std::memcpy(content, bolbBuffer, copyByteSize));
}

torch::Dtype ConvertDatatype(const tensorflow::DataType dtype) {
  switch (dtype) {
    case tensorflow::DataType::DT_INT8:
      return torch::kChar;
    case tensorflow::DataType::DT_UINT8:
      return torch::kByte;
    case tensorflow::DataType::DT_INT16:
      return torch::kShort;
    case tensorflow::DataType::DT_INT32:
      return torch::kInt;
    case tensorflow::DataType::DT_INT64:
      return torch::kLong;
    case tensorflow::DataType::DT_FLOAT:
      return torch::kFloat;
    case tensorflow::DataType::DT_DOUBLE:
      return torch::kDouble;
    default:
      return torch::kFloat;
  }
}

bool ConvertDims(const std::vector<int>& shape, adlik::serving::DimsList& dims) {
  dims.Clear();
  for (size_t i = 1; i < shape.size(); ++i) {
    dims.Add(shape[i]);
  }
  return true;
}

}  // namespace libtorch_runtime