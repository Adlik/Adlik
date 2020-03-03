// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H005764D3_3958_49F0_A5D4_EDE849D88268
#define H005764D3_3958_49F0_A5D4_EDE849D88268

#include <string>

#include "adlik_serving/framework/domain/model_spec.pb.h"
#include "cub/base/comparator.h"
#include "cub/base/optional.h"

namespace adlik {
namespace serving {

struct ModelId;

struct ModelRequest {
  static ModelRequest specific(const std::string&, int);
  static ModelRequest specific(const ModelId&);
  static ModelRequest earliest(const std::string&);
  static ModelRequest latest(const std::string&);
  static ModelRequest from(const ModelSpec&);
  static ModelRequest label(const std::string& label, const std::string& name);

  DECL_EQUALS(ModelRequest);
  DECL_LESS(ModelRequest);

  std::size_t hash() const noexcept;

private:
  enum AutoVersionPolicy {
    EARLIEST,
    LATEST,
  };

  ModelRequest(const std::string& name, int version);
  ModelRequest(const std::string& name, AutoVersionPolicy policy);

  std::size_t hashPolicy() const noexcept;
  std::size_t hashVersion() const noexcept;

private:
  std::string name;
  cub::Optional<int> version;
  AutoVersionPolicy policy;
};

}  // namespace serving
}  // namespace adlik

namespace std {
template <>
struct hash<adlik::serving::ModelRequest> {
  size_t operator()(const adlik::serving::ModelRequest& req) const noexcept {
    return req.hash();
  }
};
}  // namespace std

#endif
