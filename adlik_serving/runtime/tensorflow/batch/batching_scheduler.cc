#include "adlik_serving/runtime/tensorflow/batch/batching_scheduler.h"
#include "adlik_serving/runtime/tensorflow/batch/batching_parameters.h"
#include "adlik_serving/runtime/tensorflow/batch/batching_processor.h"
#include "adlik_serving/runtime/tensorflow/batch/inferential_batch.h"
#include "adlik_serving/runtime/tensorflow/model/model_signature.h"

namespace tensorflow {

Status BatchingScheduler::config() {
  return ROLE(BatchingParameters).make(scheduler);
}

inline SharedBatcher::QueueOptions BatchingScheduler::options() const {
  return ROLE(BatchingParameters).getQueueOptions();
}

Status BatchingScheduler::append(const ModelSignature& signature, BatchingProcessor& processor, UniqueBatcher& queue) {
  auto process = [signature, &processor](auto batch) { processor.process(signature, InferentialBatch(*batch)); };
  return scheduler->AddQueue(options(), process, &queue);
}

}  // namespace tensorflow
