#include "adlik_serving/runtime/util/shared_batcher_wrapper.h"

#include "cub/log/log.h"

namespace adlik {
namespace serving {

void SharedBatcherWrapper::config(const std::string& name) {
  Batcher::Options options;
  options.thread_pool_name = name + "_batch_threads";

  auto status = Batcher::Create(options, &batch_scheduler);
  if (!status.ok()) {
    FATAL_LOG << "Runtime " << name << " create shared scheduler error: " << status.error_message();
    return;
  }
}

tensorflow::Status SharedBatcherWrapper::append(QueueOptions& queue_options,
                                                BatchProcessor processor,
                                                std::unique_ptr<Queue>* queue) {
  return batch_scheduler->AddQueue(queue_options, processor, queue);
}

}  // namespace serving
}  // namespace adlik
