#ifndef H071EAA22_9EA1_4A33_BD1D_67239579DCBD
#define H071EAA22_9EA1_4A33_BD1D_67239579DCBD

#include "adlik_serving/runtime/tensorflow/factory/tf_plan_model_config.pb.h"
#include "cub/dci/role.h"
#include "tensorflow/core/lib/core/status.h"

namespace tensorflow {

class SavedModelBundle;
class SessionOptions;
class RunOptions;

struct TfPlanModelOptions;

DEFINE_ROLE(TfPlanModelConfig) EXTENDS(TfPlanModelConfigProto) {
  void config(const TfPlanModelOptions&);

  bool batching() const;
  Status load(const std::string& path, SavedModelBundle&) const;

private:
  using ModelTags = std::unordered_set<std::string>;

  ModelTags getModelTags() const;
  const std::string& getModelPath() const;
  SessionOptions getSessionOptions() const;
  RunOptions getRunOptions() const;
};

}  // namespace tensorflow

#endif
