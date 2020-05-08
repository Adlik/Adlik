// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/protobuf/text_protobuf.h"

#include "cub/env/env.h"
#include "google/protobuf/text_format.h"

namespace cub {

TextProtobuf::TextProtobuf(const std::string& name) {
  if (auto file = filesystem().mmap(name)) {
    region.reset(file);
  }
}

bool TextProtobuf::parse(google::protobuf::Message& msg) {
  using google::protobuf::TextFormat;
  return region && TextFormat::ParseFromString(region->buff(), &msg);
}

}  // namespace cub
