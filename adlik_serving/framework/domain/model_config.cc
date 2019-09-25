#include "adlik_serving/framework/domain/model_config.h"

#include "adlik_serving/framework/domain/model_id.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

namespace {
tensorflow::Status addLabel(const std::string& model_dir, const ModelConfigProto& config, LabelProvider& provider) {
  for (const auto& io : config.output()) {
    if (!io.label_filename().empty()) {
      const auto label_path = cub::paths(model_dir, io.label_filename());
      TF_RETURN_IF_ERROR(provider.addLabels(io.name(), label_path));
    }
  }
  return tensorflow::Status::OK();
}

}  // namespace
ModelConfig::ModelConfig(const std::string& base, const std::string& name, ModelConfigProto&& config)
    : ModelConfigProto(std::move(config)), basepath(cub::paths(base, name)), model_name(name), label_provider() {
  auto status = addLabel(getBasePath(), *this, label_provider);
  if (!status.ok()) {
    ERR_LOG << "Add model label fail, status: " << status.error_message();
  }
}

const std::string& ModelConfig::getModelName() const {
  return model_name;
}

const std::string& ModelConfig::getBasePath() const {
  return basepath;
}

std::string ModelConfig::getModelPath(const ModelId& id) const {
  return cub::paths(getBasePath(), std::to_string(id.getVersion()));
}

const LabelProvider& ModelConfig::getLabelProvider() const {
  return label_provider;
}

}  // namespace serving
}  // namespace adlik
