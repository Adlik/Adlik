// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H9E9DB76F_4969_47B8_969F_5DBF8FFFB313
#define H9E9DB76F_4969_47B8_969F_5DBF8FFFB313

#include <string>
#include <unordered_map>

#include "adlik_serving/framework/domain/model_config.h"
#include "cub/env/concurrent/mutex.h"
#include "cub/env/fs/file_system.h"

namespace adlik {
namespace serving {

struct ModelOptions;

DEFINE_ROLE(ModelConfigVisitor) {
  ABSTRACT(void visit(const ModelConfig&));
};

struct ModelStore : private cub::DirentVisitor {
  cub::Status config();
  cub::Status configModel(const std::string& name);
  cub::Status deleteModel(const std::string& name);
  void models(ModelConfigVisitor&) const;
  void models(const std::string& name, ModelConfigVisitor&) const;
  const ModelConfig* find(const std::string& name) const;
  bool exist(const std::string& name) const;
  cub::Status updatePolicy(const std::string& name, const VersionPolicyProto& policy);

private:
  OVERRIDE(void visit(const std::string&, const std::string&));

private:
  mutable cub::Mutex mu;
  std::unordered_map<std::string, ModelConfig> configs;

private:
  USE_ROLE(ModelOptions);
};

}  // namespace serving
}  // namespace adlik

#endif
