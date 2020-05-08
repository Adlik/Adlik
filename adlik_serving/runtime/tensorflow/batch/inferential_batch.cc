// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/inferential_batch.h"

#include "adlik_serving/runtime/tensorflow/batch/batching_parameters.h"
#include "adlik_serving/runtime/tensorflow/batch/merged_dimensions.h"
#include "adlik_serving/runtime/tensorflow/batch/merged_inputs.h"
#include "adlik_serving/runtime/tensorflow/batch/splited_outputs.h"
#include "adlik_serving/runtime/tensorflow/batch/splited_sizes.h"
#include "adlik_serving/runtime/tensorflow/model/errors.h"
#include "adlik_serving/runtime/tensorflow/model/model_signature.h"
#include "cub/env/time/time.h"
#include "tensorflow/core/public/session.h"

namespace tensorflow {

InferentialBatch::InferentialBatch(serving::Batch<InferentialTask>& batch) : batch(batch) {
}

namespace {
struct RealPadding : FinalPadding {
  RealPadding(int size) : size(size) {
  }

private:
  OVERRIDE(void fill(const Tensor& padding, MergedTensors& merged) const) {
    auto copy = padding.Slice(0, 1);
    for (int i = 0; i < size; ++i) {
      merged.append(copy);
    }
  }

private:
  int size;
};

struct BatchProcessor {
  BatchProcessor(serving::Batch<InferentialTask>& batch,
                 const ModelSignature& signature,
                 Session& session,
                 BatchingParameters& config)
      : batch(batch), signature(signature), session(session), config(config) {
  }

  void process() {
    batch.WaitUntilClosed();
    Status status = Status::OK();
    if (!batch.empty()) {
      status = doprocess(cub::timesystem().nowMicros());
    }
    done(status);
  }

private:
  Status doprocess(uint64_t now) {
    TF_ASSERT_FALSE(allTimeout(now));

    InputTensors inputs;
    TF_ASSERT_SUCC_CALL(mergeInputs(inputs));

    OutputTensors outputs;
    RunMetadata meta;
    TF_ASSERT_SUCC_CALL(session.Run(options(now), inputs, signature.getOutputs(), {}, &outputs, &meta));

    TF_ASSERT_SUCC_CALL(splitOutputs(outputs));

    splitMetadata(meta);

    return Status::OK();
  }

private:
  Status mergeInputs(InputTensors& mergeds) {
    MergedDimensions dims;
    collectDims(dims);

    MergedInputs inputs(config.shouldPadding(), dims);
    mergeInputs(inputs);

    return inputs.merge(signature, mergeds);
  }

  Status splitOutputs(OutputTensors& mergeds) {
    SplitedSizes sizes;
    collectSizes(sizes);

    SplitedOuputs outputs(sizes);
    TF_ASSERT_SUCC_CALL(outputs.split(signature.getOutputs(), mergeds));
    TF_ASSERT_SUCC_CALL(dispatchOutputs(outputs));
    return Status::OK();
  }

  void collectSizes(SplitedSizes& sizes) {
    tasks([&sizes](auto& task) { sizes.add(task.size()); });

    auto size = config.getPaddingSize(batch.size());
    if (size > 0) {
      sizes.add(size);
    }
  }

  Status dispatchOutputs(const SplitedOuputs& outputs) const {
    for (auto i = 0; i != batch.num_tasks(); ++i) {
      TF_ASSERT_SUCC_CALL(dispatchByTask(outputs, *batch.mutable_task(i), i));
    }
    return Status::OK();
  }

  Status dispatchByTask(const SplitedOuputs& outputs, TaskOutputs& task, int id) const {
    auto split = [&outputs, &task, id](auto& name) {
      if (auto tensors = outputs.find(name)) {
        task.addOutput(tensors->at(id));
        return Status::OK();
      } else {
        return errors::NotFound("not found tensor");
      }
    };
    return task.names(split);
  }

  template <typename F>
  void tasks(F f) {
    for (auto i = 0; i < batch.num_tasks(); ++i) {
      f(*batch.mutable_task(i));
    }
  }

  template <typename F>
  void tasks(F f) const {
    for (auto i = 0; i < batch.num_tasks(); ++i) {
      f(batch.task(i));
    }
  }

  template <typename Pred>
  bool allof(Pred pred) const {
    for (auto i = 0; i < batch.num_tasks(); ++i) {
      if (!pred(batch.task(i)))
        return false;
    }
    return true;
  }

  inline uint64 deadline() const {
    uint64 result = 0;
    tasks([&result](auto& task) {
      if (task.deadline() > result) {
        result = task.deadline();
      }
    });
    return result;
  }

  inline uint64 remained(uint64 dequeueTime) const {
    auto time = deadline();
    return time != INT_MAX ? (time - dequeueTime) / 1000 : 0;
  }

  void done(Status& status) {
    tasks([&status](TaskResult& task) { task.done(status); });
  }

  template <typename F>
  void inputs(F f) const {
    for (auto i = 0; i != batch.num_tasks(); ++i) {
      auto last = i == batch.num_tasks() - 1;
      batch.task(i).inputs([f, last](auto& name, auto& tensor) { f(name, tensor, last); });
    }
  }

  void collectDims(MergedDimensions& dims) const {
    inputs([&dims](auto& name, auto& tensor, bool) { dims.append(name, tensor); });
  }

  void mergeInputs(MergedInputs& ins) const {
    auto paddingSize = config.getPaddingSize(batch.size());
    inputs([&ins, paddingSize](auto& name, auto& tensor, bool last) {
      RealPadding padding(paddingSize);
      ins.append(name, tensor, (last ? padding : FinalPadding::nil()));
    });
  }

  bool allTimeout(uint64_t now) const {
    return allof([now](const TaskOptions& task) { return task.wasTimeout(now); });
  }

  RunOptions options(uint64 now) const {
    auto opts = batch.task(0).options();
    opts.set_timeout_in_ms(remained(now));
    return opts;
  }

  void splitMetadata(const RunMetadata& meta) {
    tasks([&meta](TaskOutputs& task) { task.metadata(meta); });
  }

private:
  serving::Batch<InferentialTask>& batch;
  const ModelSignature& signature;
  Session& session;
  BatchingParameters& config;
};
}  // namespace

// scheduler will close batch when batch size reached or timeout.
void InferentialBatch::process(const ModelSignature& signature, Session& session, BatchingParameters& config) const {
  BatchProcessor processor(batch, signature, session, config);
  processor.process();
}

}  // namespace tensorflow
