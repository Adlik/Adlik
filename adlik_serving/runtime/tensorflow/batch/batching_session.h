// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HD4EA67C3_E52A_4121_8267_255FF1E93F6C
#define HD4EA67C3_E52A_4121_8267_255FF1E93F6C

#include <unordered_map>

#include "adlik_serving/runtime/tensorflow/batch/atomic_batcher.h"
#include "adlik_serving/runtime/tensorflow/batch/batching_processor.h"
#include "adlik_serving/runtime/tensorflow/batch/serving_session.h"
#include "adlik_serving/runtime/tensorflow/model/model_signature.h"

namespace adlik {
namespace serving {
struct ModelConfig;
}
}  // namespace adlik

namespace tensorflow {

struct MetaGraph;
struct BatchingScheduler;
struct BatchingParameters;

struct BatchingSession : private BatchingProcessor, ServingSession {
  BatchingSession(Session&);

  void config(const adlik::serving::ModelConfig&);

private:
  OVERRIDE(Status Run(const RunOptions&,
                      const InputTensors&,
                      const OutputNames&,
                      const TargetNames&,
                      OutputTensors*,
                      RunMetadata*));
  OVERRIDE(Status ListDevices(std::vector<DeviceAttributes>* rsp));

private:
  OVERRIDE(void process(const ModelSignature&, const InferentialBatch&));

private:
  template <typename... Params>
  Status batch(UniqueBatcher&, Params&&...);

private:
  Session& session;
  std::unordered_map<ModelSignature, UniqueBatcher> schedulers;

private:
  USE_ROLE(MetaGraph);
  USE_ROLE(BatchingScheduler);
  USE_ROLE(BatchingParameters);
};

}  // namespace tensorflow

#endif
