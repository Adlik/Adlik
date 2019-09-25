#include "adlik_serving/framework/domain/model_path.h"

namespace adlik {
namespace serving {

ModelPath::ModelPath(const std::string& base, const std::string& name, int version)
    : ModelPath(base + name + std::to_string(version)) {
}

ModelPath::ModelPath(const std::string& fullPath) : fullPath(fullPath) {
}

const std::string& ModelPath::getFullPath() const {
  return fullPath;
}

}  // namespace serving
}  // namespace adlik
