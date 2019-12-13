// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/apis/predict_impl.h"

#include <algorithm>
#include <functional>
#include <numeric>

#include "adlik_serving/apis/predict.pb.h"
#include "adlik_serving/framework/domain/model_store.h"
#include "adlik_serving/framework/manager/managed_store.h"
#include "adlik_serving/framework/manager/run_options.h"
#include "adlik_serving/framework/manager/runtime_suite.h"
#include "adlik_serving/framework/manager/serving_store.h"
#include "adlik_serving/framework/manager/time_stats.h"
#include "cub/log/log.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

namespace {

using LabelGetter = std::function<const std::string&(size_t index)>;

template <typename T>
void addClassResults(PredictResponse_Output& output,
                     const size_t batch_size,
                     const size_t expected_class_count,
                     LabelGetter label_provider) {
  const tensorflow::TensorProto& tensor = output.tensor();
  const size_t entry_cnt = tensor.tensor_content().size() / (batch_size * sizeof(T));
  if (entry_cnt <= 0) {
    DEBUG_LOG << "tensor content size is 0, can't set classes info";
    return;
  }

  DEBUG_LOG << "addClassResults: batch size: " << batch_size << ", class count: " << entry_cnt;

  const T* probs = reinterpret_cast<const T*>(tensor.tensor_content().c_str());
  const size_t class_cnt = std::min((size_t)expected_class_count, entry_cnt);

  std::vector<size_t> idx(entry_cnt);
  for (size_t i = 0; i < batch_size; ++i) {
    // Do one sample's classification
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&probs](size_t i1, size_t i2) { return probs[i1] > probs[i2]; });
    auto& classes = *output.add_batch_classes();
    for (size_t k = 0; k < class_cnt; ++k) {
      auto cls = classes.add_cls();
      cls->set_idx((::google::protobuf::int32)idx[k]);
      cls->set_score(static_cast<float>(probs[idx[k]]));
      cls->set_label(label_provider(idx[k]));
    }
    probs += entry_cnt;
  }
}

tensorflow::Status postProcessOutputs(const PredictRequest& req,
                                      const ModelConfig& model_config,
                                      PredictResponse& rsp) {
  for (auto& it : req.output_filter()) {
    if (!it.second.has_cls())
      continue;

    auto func = [&](size_t index) -> decltype(auto) {
      return model_config.getLabelProvider().getLabel(it.first, index);
    };

    auto& output = (*rsp.mutable_outputs())[it.first];
    tensorflow::DataType dtype = output.tensor().dtype();

#define ADD_CLASS(dtype, rtype)                                                      \
  case tensorflow::DataType::dtype:                                                  \
    addClassResults<rtype>(output, req.batch_size(), it.second.cls().count(), func); \
    break

    switch (dtype) {
      ADD_CLASS(DT_UINT8, uint8_t);
      ADD_CLASS(DT_UINT16, uint16_t);
      ADD_CLASS(DT_UINT32, uint32_t);
      ADD_CLASS(DT_UINT64, uint64_t);

      ADD_CLASS(DT_INT8, int8_t);
      ADD_CLASS(DT_INT16, int16_t);
      ADD_CLASS(DT_INT32, int32_t);
      ADD_CLASS(DT_INT64, int64_t);

      ADD_CLASS(DT_FLOAT, float);
      ADD_CLASS(DT_DOUBLE, double);

      default:
        return tensorflow::errors::InvalidArgument("class result not available for output '",
                                                   it.first,
                                                   "' due to unsupported type '",
                                                   tensorflow::DataType_Name(dtype),
                                                   "'");
    }
  }
  return tensorflow::Status::OK();
}

}  // namespace

tensorflow::Status PredictImpl::predict(const RunOptions& options, const PredictRequest& req, PredictResponse& rsp) {
  std::unique_ptr<ModelHandle> handle = ROLE(ServingStore).find(req.model_spec());
  auto config = ROLE(ModelStore).find(req.model_spec().name());
  if (handle && config) {
    if (auto runtime = RuntimeSuite::inst().get(config->platform())) {
      tensorflow::Status status;
      {
        TimeStats stats("PredictServiceImpl::predict: process request, model " + req.model_spec().name() +
                        ", batch size " + std::to_string(req.batch_size()));
        status = runtime->predict(options, handle.get(), req, rsp);
      }

      DEBUG_LOG << "After predict, code: " << status.code() << ", status: " << status.error_message();
      if (status.ok()) {
        if (auto model_config = ROLE(ModelStore).find(req.model_spec().name())) {
          status = postProcessOutputs(req, *model_config, rsp);
          DEBUG_LOG << "After postProcessOutputs, status: " << status.error_message() << ", code: " << status.code();
        } else {
          ERR_LOG << "Not found model config when postprocess predict response, "
                     "model name="
                  << req.model_spec().name();
        }
      }
      return status;
    }
  }
  ERR_LOG << "Not found model or runtime by name " << req.model_spec().name();
  return tensorflow::errors::InvalidArgument("Not found model or runtime by name ", req.model_spec().name());
}

tensorflow::Status PredictImpl::predict(const PredictRequest& req, PredictResponse& rsp) {
  RunOptions options;
  return predict(options, req, rsp);
}

}  // namespace serving
}  // namespace adlik
