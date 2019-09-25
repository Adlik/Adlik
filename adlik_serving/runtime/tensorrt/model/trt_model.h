#ifndef ADLIK_SERVING_RUNTIME_TENSORRT_MODEL_TRT_MODEL_H_
#define ADLIK_SERVING_RUNTIME_TENSORRT_MODEL_TRT_MODEL_H_

#include <memory>

#include "adlik_serving/framework/domain/model_config.h"
#include "adlik_serving/framework/domain/model_id.h"
#include "adlik_serving/runtime/batching/batching_model.h"
#include "adlik_serving/runtime/batching/composite_batch_processor.h"
#include "cub/base/status.h"

namespace tensorrt {

struct TrtModel : adlik::serving::CompositeBatchProcessor, adlik::serving::BatchingModel {
  static cub::Status create(const adlik::serving::ModelConfig&,
                            const adlik::serving::ModelId&,
                            std::unique_ptr<TrtModel>*);

private:
  TrtModel(const adlik::serving::ModelConfig& config, const adlik::serving::ModelId& model_id);

  tensorflow::Status init();

  IMPL_ROLE_NS(adlik::serving, BatchProcessor)

  adlik::serving::ModelConfig config;
  adlik::serving::ModelId model_id;
};

}  // namespace tensorrt

#endif
