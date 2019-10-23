// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H2C44DF4E_FD92_4E4A_B3A8_AF95C5227B78
#define H2C44DF4E_FD92_4E4A_B3A8_AF95C5227B78

#include "adlik_serving/framework/domain/label_provider.h"
#include "adlik_serving/framework/domain/model_config.pb.h"

namespace adlik {
namespace serving {

struct ModelId;

struct ModelConfig : ModelConfigProto {
  ModelConfig(const std::string& base, const std::string& name, ModelConfigProto&&);

  const std::string& getModelName() const;
  const std::string& getBasePath() const;
  std::string getModelPath(const ModelId& id) const;
  const LabelProvider& getLabelProvider() const;

private:
  std::string basepath;
  std::string model_name;
  LabelProvider label_provider;
};

}  // namespace serving
}  // namespace adlik

#endif
