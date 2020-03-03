// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/manager/storage_loop.h"

#include "adlik_serving/framework/domain/model_options.h"
#include "adlik_serving/framework/domain/model_store.h"
#include "adlik_serving/framework/domain/model_stream.h"
#include "adlik_serving/framework/domain/model_target.h"
#include "cub/env/fs/file_system.h"
#include "cub/log/log.h"
#include "cub/string/str_utils.h"

namespace adlik {
namespace serving {

namespace {
struct VersionSelector : cub::DirentVisitor {
  VersionList select(const VersionPolicyProto& policy) const {
    VersionList result;
    versions.select(policy, result);
    return result;
  }

private:
  OVERRIDE(void visit(const std::string&, const std::string& child)) {
    uint32_t version = 1;
    if (cub::strutils::to_uint32(child, version)) {
      versions.add(version);
    }
  }

private:
  VersionList versions;
};

struct ModelStreamList : ModelConfigVisitor {
  void poll(ModelTarget& target) {
    for (auto& ms : ss) {
      // INFO_LOG << "model stream boarding: " << ms.str();
      target.update(ms);
    }
  }

private:
  OVERRIDE(void visit(const ModelConfig& model)) {
    VersionSelector selector;
    cub::filesystem().children(model.getBasePath(), selector);
    ss.emplace_back(model.getModelName(), selector.select(model.version_policy()));
  }

private:
  std::vector<ModelStream> ss;
};
}  // namespace

void StorageLoop::poll(ModelTarget& target) {
  ModelStreamList streams;
  ROLE(ModelStore).models(streams);
  streams.poll(target);
}

int64_t StorageLoop::interval() const {
  return ROLE(ModelOptions).getIntervalMs();
}

void StorageLoop::connect(ModelTarget& target) {
  auto action = [this, &target] { this->poll(target); };
  loop.reset(new cub::LoopThread(action, interval()));
}

}  // namespace serving
}  // namespace adlik
