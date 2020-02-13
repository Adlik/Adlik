// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/factory/tf_plan_model_config.h"

#include "adlik_serving/runtime/tensorflow/factory/tf_plan_model_options.h"
#include "adlik_serving/runtime/tensorflow/model/plan_model.h"
#include "tensorflow/cc/saved_model/loader.h"
#include "tensorflow/cc/saved_model/tag_constants.h"
#include "tensorflow/core/framework/tensor_util.h"
#include "tensorflow_serving/session_bundle/session_bundle_util.h"

namespace tensorflow {

inline auto TfPlanModelConfig::getModelTags() const -> ModelTags {
  ModelTags tags(saved_model_tags().begin(), saved_model_tags().end());
  if (tags.empty()) {
    tags.insert(kSavedModelTagServe);
  }
  return tags;
}

inline SessionOptions TfPlanModelConfig::getSessionOptions() const {
  SessionOptions opts;
  opts.target = session_target();
  opts.config = session_config();
  return opts;
}

inline RunOptions TfPlanModelConfig::getRunOptions() const {
  RunOptions opts;
  if (has_session_run_load_threadpool_index()) {
    opts.set_inter_op_thread_pool(session_run_load_threadpool_index().value());
  }
  return opts;
}

Status TfPlanModelConfig::load(const std::string& path, SavedModelBundle& bundle) const {
  return serving::session_bundle::LoadSessionBundleOrSavedModelBundle(
      getSessionOptions(), getRunOptions(), path, getModelTags(), &bundle);
}

bool TfPlanModelConfig::batching() const {
  return has_batching_parameters();
}

void TfPlanModelConfig::config(const TfPlanModelOptions& opts) {
  if (opts.shouldBatching()) {
    opts.batch(*mutable_batching_parameters());
  }
  opts.config(*mutable_session_config());
  opts.tags([this](auto& tag) { this->add_saved_model_tags(tag); });
  set_enable_model_warmup(opts.shouldWarmup());
}

}  // namespace tensorflow
