// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H6C0090BC_36C3_421C_99F6_92D995F0EFF7
#define H6C0090BC_36C3_421C_99F6_92D995F0EFF7

#include <string>
#include <unordered_map>

#include "adlik_serving/framework/manager/model_factory.h"

namespace adlik {
namespace serving {

struct ModelFactorySuite : ModelFactory {
  void add(const std::string&, ModelFactory&);

private:
  OVERRIDE(Model* create(const ModelId&, const ModelConfig&));

private:
  std::unordered_map<std::string, ModelFactory*> factories;
};

}  // namespace serving
}  // namespace adlik

#endif
