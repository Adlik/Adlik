// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/model_stream.h"

#include "adlik_serving/framework/domain/model_id.h"

namespace adlik {
namespace serving {

ModelStream::ModelStream(const std::string& name, const VersionList& list) : name(name), list(list) {
}

bool ModelStream::matches(const std::string& modelName) const {
  return name == modelName;
}

bool ModelStream::contains(int version) const {
  return list.contains(version);
}

bool ModelStream::contains(const std::string& name, int version) const {
  return matches(name) && contains(version);
}

const std::string& ModelStream::getName() const {
  return name;
}

std::string ModelStream::str() const {
  return name + "[" + list.str() + "]";
}

}  // namespace serving
}  // namespace adlik
