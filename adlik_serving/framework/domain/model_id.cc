// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/model_id.h"

#include <ostream>

#include "cub/base/hash.h"

namespace adlik {
namespace serving {

ModelId::ModelId() : version(-1) {
}

ModelId::ModelId(const std::string& name, int version) : name(name), version(version) {
}

DEF_EQUALS(ModelId) {
  return FIELD_EQ(name) && FIELD_EQ(version);
}

DEF_LESS(ModelId) {
  if (auto result = name.compare(rhs.name)) {
    return result < 0;
  } else {
    return FIELD_LT(version);
  }
}

const std::string& ModelId::getName() const {
  return name;
}

int ModelId::getVersion() const {
  return version;
}

std::size_t ModelId::hash() const noexcept {
  std::size_t seed = 0xDEEEFFAE;
  cub::hash_combine(seed, name);
  cub::hash_combine(seed, version);
  return seed;
}

std::string ModelId::to_s() const {
  using namespace std::string_literals;
  return "{name: "s + name + ", " + "version: " + std::to_string(version) + "}";
}

std::ostream& operator<<(std::ostream& os, const ModelId& id) {
  return os << id.to_s();
}

}  // namespace serving
}  // namespace adlik
