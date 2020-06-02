// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/version_list.h"

#include <algorithm>

#include "adlik_serving/framework/domain/version_policy.pb.h"
namespace adlik {
namespace serving {

VersionList::VersionList(const std::vector<int>& versions) : versions(versions) {
}

void VersionList::add(int version) {
  versions.push_back(version);
  std::sort(versions.begin(), versions.end());
}

inline void VersionList::latest(int max, VersionList& result) const {
  auto num = 0;
  for (auto i = versions.rbegin(); i != versions.rend() && num != max; ++i, ++num) {
    result.add(*i);
  }
}

inline void VersionList::all(VersionList& result) const {
  result = versions;
}

inline void VersionList::specific(const std::set<int>& set, VersionList& list) const {
  for (auto v : versions) {
    if (set.count(v))
      list.add(v);
  };
}

namespace {
inline uint32_t num(const VersionPolicyProto& policy) {
  return std::max(policy.latest().num_versions(), 1U);
}

inline std::set<int> set(const VersionPolicyProto& policy) {
  auto begin = policy.specific().versions().begin();
  auto end = policy.specific().versions().end();
  return begin && end ? std::set<int>(begin, end) : std::set<int>();
}
}  // namespace

void VersionList::select(const VersionPolicyProto& policy, VersionList& result) const {
  switch (policy.policy_choice_case()) {
    case VersionPolicyProto::POLICY_CHOICE_NOT_SET:
    case VersionPolicyProto::kLatest:
      latest(num(policy), result);
      break;
    case VersionPolicyProto::kAll:
      all(result);
      break;
    case VersionPolicyProto::kSpecific:
      specific(set(policy), result);
      break;
    default:
      break;
  }
}

bool VersionList::contains(int version) const {
  for (auto v : versions) {
    if (version == v)
      return true;
  }
  return false;
}

std::string VersionList::str() const {
  std::string result;
  list([&result](auto version) { result += std::to_string(version) + ","; });
  return result;
}

}  // namespace serving
}  // namespace adlik
