// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef PROTOBUF_TEXT_PROTOBUF_SAVER
#define PROTOBUF_TEXT_PROTOBUF_SAVER

#include <fstream>

namespace google {
namespace protobuf {
class Message;
}
}  // namespace google

namespace cub {

struct TextProtobufSaver {
  TextProtobufSaver(const std::string& file);
  bool save(google::protobuf::Message& message);

private:
  std::ofstream outStream;
  bool isSaved;
};

}  // namespace cub

#endif
