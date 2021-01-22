// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef A1985BD3_F83B_4D81_9CD6_DB60AC6B627F
#define A1985BD3_F83B_4D81_9CD6_DB60AC6B627F

#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/runtime/tensorflow/batch/shared_batcher.h"

namespace tensorflow {

template <typename Option>
void config_with_dynamic_batching(const adlik::serving::ModelConfig& config, Option& options) {
  if (config.has_dynamic_batching()) {
    const auto& dynamic_batching = config.dynamic_batching();
    options.batch_timeout_micros = dynamic_batching.max_queue_delay_microseconds();
    if (dynamic_batching.max_enqueued_batches() != 0) {
      options.max_enqueued_batches = dynamic_batching.max_enqueued_batches();
    }
  }
}

template <typename Option>
void optionsFromConfig(const adlik::serving::ModelConfig& config, Option& options) {
  options.max_batch_size = config.max_batch_size();
  config_with_dynamic_batching(config, options);
}

template <>
void optionsFromConfig(const adlik::serving::ModelConfig& config, SharedBatcher::QueueOptions& options) {
  options.input_batch_size_limit = config.max_batch_size();
  config_with_dynamic_batching(config, options);
}

}  // namespace tensorflow

#endif
