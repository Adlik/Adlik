#ifndef ADLIK_SERVING_RUNTIME_OPENVINO_MODEL_OPENVINO_UTIL_H
#define ADLIK_SERVING_RUNTIME_OPENVINO_MODEL_OPENVINO_UTIL_H

#include <inference_engine.hpp>

#include "adlik_serving/framework/domain/dims_list.h"
#include "adlik_serving/framework/domain/model_config.pb.h"
#include "adlik_serving/runtime/openvino/openvino.h"
#include "tensorflow/core/lib/core/status.h"

OPENVINO_NS_BEGIN

InferenceEngine::Blob::Ptr getBlob(InferenceEngine::Precision precision,
                                   InferenceEngine::SizeVector dims,
                                   InferenceEngine::Layout layout);

InferenceEngine::Layout ConvertToOpenVinoLayout(adlik::serving::ModelInput_Format format);

InferenceEngine::Precision ConvertToOpenVinoDataType(const tensorflow::DataType dtype);

tensorflow::DataType ConvertToTensorflowDatatype(InferenceEngine::Precision openvinoType);

bool ConvertDims(const InferenceEngine::SizeVector& openvinDim, adlik::serving::DimsList& dims);

bool copyBlob2Buffer(void* content, InferenceEngine::Blob::Ptr blobPtr, size_t copyByteSize, size_t offsetByteSize);

bool copyBuffer2Blob(const void* content,
                     InferenceEngine::Blob::Ptr blobPtr,
                     size_t copyByteSize,
                     size_t offsetByteSize);

OPENVINO_NS_END

#endif
