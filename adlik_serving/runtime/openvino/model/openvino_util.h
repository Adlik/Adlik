// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_OPENVINO_MODEL_OPENVINO_UTIL_H
#define ADLIK_SERVING_RUNTIME_OPENVINO_MODEL_OPENVINO_UTIL_H

#include <openvino/openvino.hpp>

#include "adlik_serving/framework/domain/dims_list.h"
#include "adlik_serving/framework/domain/model_config.pb.h"
#include "adlik_serving/runtime/openvino/openvino.h"
#include "tensorflow/core/lib/core/status.h"

OPENVINO_NS_BEGIN

tensorflow::DataType ConvertToTensorflowDatatype(ov::element::Type openvinoType);

bool ConvertDims(const ov::Shape& ovshape, adlik::serving::DimsList& dims);

bool copyBlob2Buffer(void* content, const ov::Tensor& tensor, size_t copyByteSize, size_t offsetByteSize);

bool copyBuffer2Blob(const void* content, const ov::Tensor& tensor, size_t copyByteSize, size_t offsetByteSize);

OPENVINO_NS_END

#endif
