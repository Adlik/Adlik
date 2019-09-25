#ifndef ADLIK_SERVING_FRAMEWORK_DOMAIN_LABEL_PROVIDER_H
#define ADLIK_SERVING_FRAMEWORK_DOMAIN_LABEL_PROVIDER_H

#include <string>
#include <unordered_map>
#include <vector>

#include "tensorflow/core/lib/core/status.h"

namespace adlik {
namespace serving {

class LabelProvider {
public:
  LabelProvider() = default;

  const std::string& getLabel(const std::string& name, size_t index) const;

  tensorflow::Status addLabels(const std::string& name, const std::string& filepath);

private:
  //   LabelProvider(const LabelProvider&) = delete;
  //   void operator=(const LabelProvider&) = delete;

  std::unordered_map<std::string, std::vector<std::string>> label_map_;
};
}  // namespace serving
}  // namespace adlik

#endif
