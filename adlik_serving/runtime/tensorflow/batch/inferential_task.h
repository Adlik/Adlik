// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HE3761470_AA55_4812_8796_B67975CB5937
#define HE3761470_AA55_4812_8796_B67975CB5937

#include "adlik_serving/runtime/tensorflow/model/errors.h"
#include "adlik_serving/runtime/tensorflow/model/model_inputs.h"
#include "adlik_serving/runtime/tensorflow/model/model_outputs.h"
#include "cub/env/concurrent/notification.h"
#include "tensorflow/core/kernels/batching_util/batch_scheduler.h"
#include "tensorflow/core/protobuf/config.pb.h"

namespace tensorflow {

struct TaskInputs {
  TaskInputs(const InputTensors&);

  bool isValid() const;
  int64 getBatchSize() const;

  template <typename F>
  void inputs(F f) const {
    for (auto& p : ins) {
      f(p.first, p.second);
    }
  }

private:
  const InputTensors& ins;
};

struct TaskOutputs {
  TaskOutputs(const OutputNames& outNames, OutputTensors& outputs, RunMetadata& meta);

  void metadata(const RunMetadata& meta);
  void addOutput(const Tensor& tensor);

  template <typename F>
  Status names(F f) const {
    for (auto& name : outNames) {
      TF_ASSERT_SUCC_CALL(f(name));
    }
    return Status::OK();
  }

private:
  const OutputNames& outNames;
  OutputTensors& outputs;
  RunMetadata& meta;
};

struct TaskResult {
  Status wait();
  void done(Status);

private:
  Status status;
  cub::Notification finished;
};

struct TaskOptions {
  TaskOptions(const RunOptions&);

  uint64 deadline() const;
  bool wasTimeout(uint64) const;
  const RunOptions& options() const;

private:
  RunOptions opts;
  uint64 start;
};

struct InferentialTask : serving::BatchTask, TaskOptions, TaskInputs, TaskOutputs, TaskResult {
  InferentialTask(const RunOptions&,
                  const InputTensors& inputs,
                  const OutputNames& names,
                  OutputTensors& outputs,
                  RunMetadata& meta);

  size_t size() const override;
};

}  // namespace tensorflow

#endif
