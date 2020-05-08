// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HFE4A4FFC_6F1B_4124_9B0D_A88B7EA9663F
#define HFE4A4FFC_6F1B_4124_9B0D_A88B7EA9663F

#include <iosfwd>
#include <string>

#include "cub/base/comparator.h"

namespace adlik {
namespace serving {

struct ModelId {
  ModelId();
  ModelId(const std::string& name, int version);

  DECL_EQUALS(ModelId);
  DECL_LESS(ModelId);

  std::size_t hash() const noexcept;
  std::string to_s() const;

  const std::string& getName() const;
  int getVersion() const;

private:
  std::string name;
  int version;
};

std::ostream& operator<<(std::ostream&, const ModelId&);

}  // namespace serving
}  // namespace adlik

namespace std {
template <>
struct hash<adlik::serving::ModelId> {
  size_t operator()(const adlik::serving::ModelId& id) const noexcept {
    return id.hash();
  }
};
}  // namespace std

#endif
