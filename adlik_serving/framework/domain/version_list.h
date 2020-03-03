// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HC5CF6164_3531_4A94_8ECA_991288AAF55C
#define HC5CF6164_3531_4A94_8ECA_991288AAF55C

#include <stdint.h>

#include <set>
#include <string>
#include <vector>

namespace adlik {
namespace serving {

class VersionPolicyProto;

struct VersionList {
  VersionList() = default;
  VersionList(const std::vector<int>&);

  void add(int version);

  void select(const VersionPolicyProto&, VersionList&) const;
  bool contains(int version) const;

  template <typename F>
  void list(F f) const {
    for (auto version : versions) {
      f(version);
    }
  }

  std::string str() const;

private:
  void latest(int num, VersionList&) const;
  void all(VersionList&) const;
  void specific(const std::set<int>& matches, VersionList&) const;

private:
  std::vector<int> versions;
};

}  // namespace serving
}  // namespace adlik

#endif
