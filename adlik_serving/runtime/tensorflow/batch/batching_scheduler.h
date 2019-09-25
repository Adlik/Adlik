#ifndef H10473F16_6FF6_4FE4_AC8D_A456DB658BED
#define H10473F16_6FF6_4FE4_AC8D_A456DB658BED

#include "adlik_serving/runtime/tensorflow/batch/atomic_batcher.h"
#include "adlik_serving/runtime/tensorflow/batch/shared_batcher.h"
#include "cub/dci/role.h"

namespace tensorflow {

struct ModelSignature;
struct BatchingProcessor;
struct BatchingParameters;

DEFINE_ROLE(BatchingScheduler) {
  Status config();
  Status append(const ModelSignature&, BatchingProcessor&, UniqueBatcher&);

private:
  SharedBatcher::QueueOptions options() const;

private:
  std::shared_ptr<SharedBatcher> scheduler;

private:
  USE_ROLE(BatchingParameters);
};

}  // namespace tensorflow

#endif
