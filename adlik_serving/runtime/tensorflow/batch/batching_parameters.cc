// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/batching_parameters.h"

namespace tensorflow {

void BatchingParameters::config(const TfPlanModelConfigProto& config) {
  this->proto = &config.batching_parameters();
}

SharedBatcher::QueueOptions BatchingParameters::getQueueOptions() const {
  SharedBatcher::QueueOptions opts;
  if (proto->has_max_batch_size()) {
    opts.input_batch_size_limit = proto->max_batch_size().value();
  }
  if (proto->has_batch_timeout_micros()) {
    opts.batch_timeout_micros = proto->batch_timeout_micros().value();
  }
  if (proto->has_max_enqueued_batches()) {
    opts.max_enqueued_batches = proto->max_enqueued_batches().value();
  }
  return opts;
}

Status BatchingParameters::make(std::shared_ptr<SharedBatcher>& batcher) {
  SharedBatcher::Options opts;
  if (proto->has_num_batch_threads()) {
    opts.num_batch_threads = proto->num_batch_threads().value();
  }
  if (proto->has_thread_pool_name()) {
    opts.thread_pool_name = proto->thread_pool_name().value();
  }
  return SharedBatcher::Create(opts, &batcher);
}

int BatchingParameters::lowest(int batchSize) const {
  if (proto->allowed_batch_sizes().empty()) {
    return batchSize;
  }
  for (auto size : proto->allowed_batch_sizes()) {
    if (size >= batchSize) {
      return size;
    }
  }
  return batchSize;
}

int BatchingParameters::getPaddingSize(int batchSize) const {
  return lowest(batchSize) - batchSize;
}

bool BatchingParameters::shouldPadding() const {
  return proto->pad_variable_length_inputs();
}

}  // namespace tensorflow
