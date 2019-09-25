#ifndef HBFF9315E_18F2_4195_8C97_7B72ED07512D
#define HBFF9315E_18F2_4195_8C97_7B72ED07512D

#include "adlik_serving/runtime/tensorflow/batch/inferential_task.h"
#include "tensorflow/core/kernels/batching_util/batch_scheduler.h"

namespace tensorflow {

class Session;

struct ModelSignature;
struct BatchingParameters;

struct InferentialBatch {
  explicit InferentialBatch(serving::Batch<InferentialTask>& batch);

  void process(const ModelSignature&, Session&, BatchingParameters&) const;

private:
  serving::Batch<InferentialTask>& batch;
};

}  // namespace tensorflow

#endif
