
// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef CUB_STRING_STRING_VIEW_HASH_H
#define CUB_STRING_STRING_VIEW_HASH_H

#include "cub/base/hash.h"
#include "cub/string/string_view.h"

namespace std {
template <>
struct hash<cub::StringView> {
  size_t operator()(const cub::StringView& view) const noexcept {
    size_t seed = 0;
    cub::hash_range(seed, view.begin(), view.end());
    return seed;
  }
};

}  // namespace std

#endif
