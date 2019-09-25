#ifndef H9E9DB76F_4969_47B8_969F_5DBF8FFFB313
#define H9E9DB76F_4969_47B8_969F_5DBF8FFFB313

#include "adlik_serving/framework/domain/model_config.h"
#include "cub/env/fs/file_system.h"

#include <string>
#include <unordered_map>

namespace adlik {
namespace serving {

struct ModelOptions;

DEFINE_ROLE(ModelConfigVisitor) {
  ABSTRACT(void visit(const ModelConfig&));
};

struct ModelStore : private cub::DirentVisitor {
  cub::Status config();

  void models(ModelConfigVisitor&) const;
  const ModelConfig* find(const std::string& name) const;

private:
  OVERRIDE(void visit(const std::string&, const std::string&));

private:
  std::unordered_map<std::string, ModelConfig> configs;

private:
  USE_ROLE(ModelOptions);
};

}  // namespace serving
}  // namespace adlik

#endif
