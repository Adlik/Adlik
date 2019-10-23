// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H300AFAA3_7124_44DF_9F5E_7C630908D8C9
#define H300AFAA3_7124_44DF_9F5E_7C630908D8C9

#include "adlik_serving/runtime/tensorflow/batch/shared_batcher.h"
#include "adlik_serving/runtime/tensorflow/factory/tf_plan_model_config.pb.h"
#include "cub/base/struct_wrapper.h"

namespace tensorflow {

struct BatchingParameters {
  void config(const TfPlanModelConfigProto& config);

  Status make(std::shared_ptr<SharedBatcher>&);

  SharedBatcher::QueueOptions getQueueOptions() const;
  int getPaddingSize(int batchSize) const;
  bool shouldPadding() const;

private:
  int lowest(int batchSize) const;

private:
  const BatchingParametersProto* proto;
};

}  // namespace tensorflow

#endif
