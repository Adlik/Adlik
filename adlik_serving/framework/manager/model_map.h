// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H5E831278_1A09_4E3A_9430_F3050BD0C5CD
#define H5E831278_1A09_4E3A_9430_F3050BD0C5CD

#include <string>
#include <unordered_map>

#include "adlik_serving/framework/domain/shared_model.h"

namespace adlik {
namespace serving {

template <typename ModelKey>
using ModelHashMap = std::unordered_multimap<ModelKey, SharedModel>;

template <typename ModelKey>
using ModelMap = std::multimap<ModelKey, SharedModel>;

}  // namespace serving
}  // namespace adlik

#endif
