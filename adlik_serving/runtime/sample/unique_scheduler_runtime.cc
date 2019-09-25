#include "adlik_serving/runtime/batching/batch_processor.h"
#include "adlik_serving/runtime/batching/batching_model.h"
#include "adlik_serving/runtime/util/unique_batcher_runtime_helper.h"

namespace sample {

// A runtime in which each model may contain multiple instances and use a basic
// batch scheduer to support batching request. The model should inherit
// "BatchProcessor" and "BatchingModel" if tread safe, otherwise it should inherit
// "CompositeBatchProcessor" and "BatchingModel" and its intance should inherit
// "BatchProcessor".

namespace {

using namespace adlik::serving;

struct MyCompositeModel : BatchProcessor, BatchingModel {
  static cub::Status create(const ModelConfig& config,
                            const ModelId& model_id,
                            std::unique_ptr<MyCompositeModel>* result) {
    auto bundle = std::make_unique<MyCompositeModel>();

    // todo: do other processes about create model

    *result = std::move(bundle);
    return cub::Success;
  }

private:
  IMPL_ROLE(BatchProcessor);
  using MyBatch = Batch<BatchingMessageTask>;
  tensorflow::Status processBatch(MyBatch&) {
    // todo: process one batch of tasks
    return tensorflow::Status::OK();
  }
};

}  // namespace

DEFINE_UNIQUE_BATCHER_RUNTIME(UniqueScheduler, MyCompositeModel);

}  // namespace sample
