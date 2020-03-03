// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/model_request.h"

#include "adlik_serving/framework/domain/model_id.h"
#include "cub/base/hash.h"

namespace adlik {
namespace serving {

ModelRequest::ModelRequest(const std::string& name, int version) : name(name), version(version), policy(LATEST) {
}

ModelRequest::ModelRequest(const std::string& name, AutoVersionPolicy policy) : name(name), policy(policy) {
}

ModelRequest ModelRequest::specific(const std::string& name, int version) {
  return {name, version};
}

ModelRequest ModelRequest::specific(const ModelId& id) {
  return {id.getName(), id.getVersion()};
}

ModelRequest ModelRequest::earliest(const std::string& name) {
  return {name, EARLIEST};
}

ModelRequest ModelRequest::latest(const std::string& name) {
  return {name, LATEST};
}

ModelRequest ModelRequest::label(const std::string& label, const std::string& name) {
  return label == "stable" ? earliest(name) : latest(name);
}

ModelRequest ModelRequest::from(const ModelSpec& spec) {
  switch (spec.version_choice_case()) {
    case ModelSpec::kVersion:
      return specific(spec.name(), spec.version().value());
    case ModelSpec::kVersionLabel:
      return label(spec.version_label(), spec.name());
    case ModelSpec::VERSION_CHOICE_NOT_SET:
    default:
      return latest(spec.name());
  }
}

DEF_EQUALS(ModelRequest) {
  return FIELD_EQ(version) && FIELD_EQ(policy) && FIELD_EQ(name);
}

DEF_LESS(ModelRequest) {
  if (auto result = name.compare(rhs.name)) {
    return result < 0;
  }
  return FIELD_LT(version);
}

inline std::size_t ModelRequest::hashPolicy() const noexcept {
  switch (policy) {
    case EARLIEST:
      return 0x1234CAFF;
    case LATEST:
      return 0xECAFCAFF;
    default:
      return 0xAFFEDECF;
  }
}

inline std::size_t ModelRequest::hashVersion() const noexcept {
  return version ? std::hash<int>()(version.value()) : hashPolicy();
}

std::size_t ModelRequest::hash() const noexcept {
  std::size_t seed = 0xEEAE8903;
  cub::hash_combine(seed, name);
  cub::hash_combine(seed, hashVersion());
  return seed;
}

}  // namespace serving
}  // namespace adlik
