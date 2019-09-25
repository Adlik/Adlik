#ifndef ADLIK_SERVING_RUNTIME_UTIL_UNIQUE_BATCHER_MODEL_WRAPPER_H
#define ADLIK_SERVING_RUNTIME_UTIL_UNIQUE_BATCHER_MODEL_WRAPPER_H

#include <functional>
#include <memory>

#include "adlik_serving/framework/domain/bare_model_loader.h"
#include "adlik_serving/framework/domain/model.h"
#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/domain/model_handle.h"
#include "adlik_serving/runtime/batching/basic_batch_scheduler.h"
#include "adlik_serving/runtime/batching/batch_scheduler.h"
#include "adlik_serving/runtime/batching/batching_message_task.h"

namespace adlik {
namespace serving {

template <typename ModelType>
struct UniqueBatcherModelWrapper : private BareModelLoader, private ModelHandle, Model {
  UniqueBatcherModelWrapper(const ModelId& id, const ModelConfig& config) : Model(id), config(config) {
  }

private:
  OVERRIDE(cub::Status loadBare()) {
    auto status = ModelType::create(config, ROLE(ModelId), &model_bundle);
    if (model_bundle) {
      createScheduler();
    }
    return status;
  }

  OVERRIDE(cub::Status unloadBare()) {
    model_bundle.reset();
    return cub::Success;
  }

  OVERRIDE(cub::AnyPtr model()) {
    return {model_bundle.get()};
  }

  using Batcher = BasicBatchScheduler<BatchingMessageTask>;

  cub::Status createScheduler() {
    typename Batcher::Options batcher_options = options();

    using BatchProcessor = std::function<void(std::unique_ptr<Batch<BatchingMessageTask>> batch)>;
    auto status = model_bundle->createScheduler(
        [&](BatchProcessor processor, std::unique_ptr<BatchScheduler<BatchingMessageTask>>* queue) {
          std::unique_ptr<BasicBatchScheduler<BatchingMessageTask>> raw_scheduler;
          auto status = BasicBatchScheduler<BatchingMessageTask>::Create(batcher_options, processor, &raw_scheduler);
          if (status.ok()) {
            *queue = std::move(raw_scheduler);
          }
          return status;
        });
    return cub::Status(status.code());
  }

  typename Batcher::Options options() {
    typename Batcher::Options options;
    options.max_batch_size = config.max_batch_size();
    options.num_batch_threads = model_bundle->count();
    options.thread_pool_name = config.name() + "_batch_threads";
    options.batch_timeout_micros = 0;  // modify this value if needed
    return options;
  }

  IMPL_ROLE(ModelHandle)
  IMPL_ROLE(ModelId)
  IMPL_ROLE(ModelState)
  IMPL_ROLE(BareModelLoader)

  const ModelConfig& config;
  std::unique_ptr<ModelType> model_bundle;
};

}  // namespace serving
}  // namespace adlik

#endif
