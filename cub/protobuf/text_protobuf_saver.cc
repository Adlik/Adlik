// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/protobuf/text_protobuf_saver.h"

#include "cub/log/log.h"
#include "google/protobuf/message.h"
#include "google/protobuf/text_format.h"
namespace cub {

TextProtobufSaver::TextProtobufSaver(const std::string& name) : outStream(name), isSaved(false) {
}

bool TextProtobufSaver::save(google::protobuf::Message& msg) {
  if (isSaved) {
    INFO_LOG << "One file can save one message only";
    return false;
  }
  using google::protobuf::TextFormat;
  std::string str;
  if (google::protobuf::TextFormat::PrintToString(msg, &str)) {
    INFO_LOG << "Message: " << str;
    isSaved = true;
  } else {
    INFO_LOG << "Message not valid (partial content: " << msg.ShortDebugString() << ")";
    return false;
  }
  outStream << str;
  return true;
}

}  // namespace cub
