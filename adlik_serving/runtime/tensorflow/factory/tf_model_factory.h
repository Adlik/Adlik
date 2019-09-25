#ifndef H52AA5541_A990_49FE_86F8_833971575B8F
#define H52AA5541_A990_49FE_86F8_833971575B8F

#include "adlik_serving/framework/manager/model_factory.h"

namespace tensorflow {

struct TfPlanModelConfig;
struct BatchingScheduler;
struct BatchingParameters;

struct TfModelFactory : adlik::serving::ModelFactory {
private:
  OVERRIDE(adlik::serving::Model* create(const adlik::serving::ModelId&, const adlik::serving::ModelConfig&));

private:
  USE_ROLE(TfPlanModelConfig);
  USE_ROLE(BatchingScheduler);
  USE_ROLE(BatchingParameters);
};

}  // namespace tensorflow

#endif
