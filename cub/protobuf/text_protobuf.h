// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HE3F35C94_F919_4143_86B5_7670718BAE11
#define HE3F35C94_F919_4143_86B5_7670718BAE11

#include "cub/base/assertions.h"
#include "cub/env/fs/file_system.h"
#include "google/protobuf/message.h"

namespace cub {

struct TextProtobuf {
  TextProtobuf(const std::string& file);

  bool parse(google::protobuf::Message& message);

  template <typename ProtoType>
  static ProtoType read(const std::string& file) {
    ProtoType proto;
    CUB_PEEK_TRUE(TextProtobuf(file).parse(proto));
    return proto;
  }

private:
  std::unique_ptr<ReadOnlyRegion> region;
};

}  // namespace cub

#endif
