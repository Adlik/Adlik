// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/factory/tf_plan_model_options.h"

#include "adlik_serving/runtime/tensorflow/factory/tf_plan_model_config.pb.h"
#include "cub/protobuf/text_protobuf.h"

namespace tensorflow {
TfPlanModelOptions::TfPlanModelOptions() : enableBatching(true), enableWarmup(true), parallelism(0), fraction(0.0f) {
}

void TfPlanModelOptions::subscribe(cub::ProgramOptions& prog) {
  auto options = new cub::OptionSet{{
      cub::option("tf_enable_batching", &enableBatching, "enable batching"),
      cub::option("tf_batching_parameters_file", &batchingFile, "batching parameters protobuf"),
      cub::option("tf_session_parallelism", &parallelism, "num of threads to run session."),
      cub::option("tf_per_process_gpu_memory_fraction",
                  &fraction,
                  "fraction that each process occupies of the GPU memory space"),
      cub::option("tf_saved_model_tags", &modelTags, "set of tags corresponding to the meta graph def"),
      cub::option("tf_enable_warmup", &enableWarmup, "enables model warmup"),
  }};
  prog.add(options);
}

void TfPlanModelOptions::batch(BatchingParametersProto& params) const {
  if (batchingFile.empty()) {
    params.mutable_thread_pool_name()->set_value("Adlik serving batch threads");
  } else {
    CUB_ASSERT_TRUE_VOID(cub::TextProtobuf(batchingFile).parse(params));
  }
}

void TfPlanModelOptions::config(ConfigProto& config) const {
  config.mutable_gpu_options()->set_per_process_gpu_memory_fraction(fraction);
  config.set_intra_op_parallelism_threads(parallelism);
  config.set_inter_op_parallelism_threads(parallelism);
}

bool TfPlanModelOptions::shouldWarmup() const {
  return enableWarmup;
}

bool TfPlanModelOptions::shouldBatching() const {
  return enableBatching;
}

}  // namespace tensorflow
