// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/core/server_core.h"
#include "adlik_serving/server/core/version.h"
#include "cub/log/log.h"

using namespace adlik::serving;

int main(int argc, const char** argv) {
  INFO_LOG << "Adlik serving version: " << version();
  return ServerCore::inst().start(argc, argv);
}
