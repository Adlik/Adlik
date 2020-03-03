// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/model_store.h"

#include "adlik_serving/framework/domain/model_options.h"
#include "cub/env/env.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"
#include "cub/protobuf/text_protobuf.h"

namespace adlik {
namespace serving {

cub::Status ModelStore::config() {
  auto& root = ROLE(ModelOptions).getBasePath();
  INFO_LOG << "model root path: " << root;
  return cub::filesystem().children(root, *this);
}

void ModelStore::visit(const std::string& base, const std::string& name) {
  ModelConfigProto proto;
  if (cub::TextProtobuf(cub::paths(base, name, "config.pbtxt")).parse(proto)) {
    INFO_LOG << "found model " << name << " [" << proto.platform() << "]";
    configs.insert({name, {base, name, std::move(proto)}});
  }
}

void ModelStore::models(ModelConfigVisitor& visitor) const {
  for (auto& i : configs) {
    visitor.visit(i.second);
  }
}

const ModelConfig* ModelStore::find(const std::string& name) const {
  auto it = configs.find(name);
  return it != configs.end() ? &it->second : nullptr;
}

}  // namespace serving
}  // namespace adlik
