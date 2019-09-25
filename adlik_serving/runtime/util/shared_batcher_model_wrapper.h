#ifndef ADLIK_SERVING_RUNTIME_UTIL_SHARED_BATCHER_MODEL_WRAPPER_H
#define ADLIK_SERVING_RUNTIME_UTIL_SHARED_BATCHER_MODEL_WRAPPER_H

#include <functional>
#include <memory>

#include "adlik_serving/framework/domain/bare_model_loader.h"
#include "adlik_serving/framework/domain/model.h"
#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/domain/model_handle.h"
#include "adlik_serving/runtime/batching/batch_scheduler.h"
#include "adlik_serving/runtime/util/shared_batcher_wrapper.h"

namespace adlik {
namespace serving {

template <typename ModelType>
struct SharedBatcherModelWrapper : private BareModelLoader, private ModelHandle, Model {
public:
  SharedBatcherModelWrapper(const ModelId& id, const ModelConfig& config, SharedBatcherWrapper& scheduler)
      : Model(id), config(config), scheduler(scheduler) {
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

  cub::Status createScheduler() {
    SharedBatcherWrapper::QueueOptions queue_options = queueOptions();

    using BatchProcessor = std::function<void(std::unique_ptr<Batch<BatchingMessageTask>> batch)>;
    auto status = model_bundle->createScheduler(
        [&](BatchProcessor processor, std::unique_ptr<BatchScheduler<BatchingMessageTask>>* queue) {
          return scheduler.append(queue_options, processor, queue);
        });
    return cub::Status(status.code());
  }

  typename SharedBatcherWrapper::QueueOptions queueOptions() {
    SharedBatcherWrapper::QueueOptions queue_options;
    queue_options.max_batch_size = this->config.max_batch_size();
    queue_options.batch_timeout_micros = 0;  // modify if need
    return queue_options;
  }

  IMPL_ROLE(ModelHandle)
  IMPL_ROLE(ModelId)
  IMPL_ROLE(ModelState)
  IMPL_ROLE(BareModelLoader)

  const ModelConfig& config;
  std::unique_ptr<ModelType> model_bundle;
  SharedBatcherWrapper& scheduler;
};

}  // namespace serving
}  // namespace adlik

#endif
