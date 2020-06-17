// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/model_store.h"

#include "adlik_serving/framework/domain/model_options.h"
#include "cub/env/concurrent/auto_lock.h"
#include "cub/env/env.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"
#include "cub/protobuf/text_protobuf.h"
#include "cub/protobuf/text_protobuf_saver.h"

namespace adlik {
namespace serving {

cub::Status ModelStore::config() {
  cub::AutoLock lock(mu);
  auto& root = ROLE(ModelOptions).getBasePath();
  INFO_LOG << "model root path: " << root;
  return cub::filesystem().children(root, *this);
}

cub::Status ModelStore::configModel(const std::string& name) {
  cub::AutoLock lock(mu);
  auto& root = ROLE(ModelOptions).getBasePath();
  INFO_LOG << "model root path: " << root << ", "
           << "model name: " << name;
  return cub::filesystem().child(root, name, *this);
}

cub::Status ModelStore::deleteModel(const std::string& name) {
  cub::AutoLock lock(mu);
  auto it = configs.find(name);
  if (it != configs.end()) {
    configs.erase(it);
  }
  return cub::Success;
}

void ModelStore::visit(const std::string& base, const std::string& name) {
  ModelConfigProto proto;
  if (cub::TextProtobuf(cub::paths(base, name, "config.pbtxt")).parse(proto)) {
    INFO_LOG << "found model " << name << " [" << proto.platform() << "]";
    configs.insert({name, {base, name, std::move(proto)}});
  } else {
    INFO_LOG << "model " << name << " config.pbtxt error";
  }
}

void ModelStore::models(ModelConfigVisitor& visitor) const {
  cub::AutoLock lock(mu);
  for (auto& i : configs) {
    visitor.visit(i.second);
  }
}

void ModelStore::models(const std::string& name, ModelConfigVisitor& visitor) const {
  auto it = configs.find(name);
  cub::AutoLock lock(mu);
  if (it != configs.end()) {
    visitor.visit(it->second);
  }
}

const ModelConfig* ModelStore::find(const std::string& name) const {
  cub::AutoLock lock(mu);
  auto it = configs.find(name);
  return it != configs.end() ? &it->second : nullptr;
}

bool ModelStore::exist(const std::string& name) const {
  cub::AutoLock lock(mu);
  auto it = configs.find(name);
  return it != configs.end() ? true : false;
}

cub::Status ModelStore::updatePolicy(const std::string& name, const VersionPolicyProto& policy) {
  cub::AutoLock lock(mu);
  auto it = configs.find(name);
  (it->second.mutable_version_policy())->CopyFrom(policy);
  ModelConfigProto proto;
  proto.CopyFrom(it->second);
  if (cub::TextProtobufSaver(cub::paths(it->second.getBasePath(), "config.pbtxt")).save(proto)) {
    INFO_LOG << "update model " << name << " policy success";
    return cub::Success;
  } else {
    return cub::Failure;
  }
}
}  // namespace serving
}  // namespace adlik
