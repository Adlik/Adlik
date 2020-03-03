// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/inferential_task.h"

#include "cub/env/time/time.h"

namespace tensorflow {

TaskInputs::TaskInputs(const InputTensors& ins) : ins(ins) {
}

bool TaskInputs::isValid() const {
  std::set<int64> dims;
  for (auto& input : ins) {
    auto& shape = input.second.shape();
    if (shape.dims() == 0)
      return false;
    else
      dims.insert(shape.dim_size(0));
  }
  return dims.size() == 1;
}

int64 TaskInputs::getBatchSize() const {
  return ins.back().second.shape().dim_size(0);
}

TaskOutputs::TaskOutputs(const OutputNames& outNames, OutputTensors& outputs, RunMetadata& meta)
    : outNames(outNames), outputs(outputs), meta(meta) {
  outputs.clear();
}

void TaskOutputs::metadata(const RunMetadata& meta) {
  this->meta = meta;
}

void TaskOutputs::addOutput(const Tensor& tensor) {
  outputs.push_back(tensor);
}

void TaskResult::done(Status status) {
  this->status = status;
  finished.notify();
}

Status TaskResult::wait() {
  finished.wait();
  return status;
}

TaskOptions::TaskOptions(const RunOptions& opts) : opts(opts), start(cub::timesystem().nowMicros()) {
}

namespace {
uint64 getTimeout(const RunOptions& opts) {
  return opts.timeout_in_ms() <= 0 ? INT_MAX : opts.timeout_in_ms() * 1000;
}
}  // namespace

uint64 TaskOptions::deadline() const {
  return start + getTimeout(opts);
}

bool TaskOptions::wasTimeout(uint64 dequeueTime) const {
  return deadline() <= dequeueTime;
}

const RunOptions& TaskOptions::options() const {
  return opts;
}

InferentialTask::InferentialTask(const RunOptions& opts,
                                 const InputTensors& inputs,
                                 const OutputNames& names,
                                 OutputTensors& outputs,
                                 RunMetadata& meta)
    : TaskOptions(opts), TaskInputs(inputs), TaskOutputs(names, outputs, meta) {
}

size_t InferentialTask::size() const {
  return TaskInputs::getBatchSize();
}

}  // namespace tensorflow
