// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_UTIL_QUEUE_OPTIONS_H
#define ADLIK_SERVING_RUNTIME_UTIL_QUEUE_OPTIONS_H

#include "adlik_serving/framework/domain/model_config.pb.h"

namespace adlik {
namespace serving {

template <typename Option>
void optionsFromConfig(const ModelConfigProto& config, Option& options) {
  options.max_batch_size = config.max_batch_size();
  if (config.has_dynamic_batching()) {
    const auto& dynamic_batching = config.dynamic_batching();
    options.batch_timeout_micros = dynamic_batching.max_queue_delay_microseconds();
    if (dynamic_batching.max_enqueued_batches() != 0) {
      options.max_enqueued_batches = dynamic_batching.max_enqueued_batches();
    }
  }
}

}  // namespace serving
}  // namespace adlik

#endif
