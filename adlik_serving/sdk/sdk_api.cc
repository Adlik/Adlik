// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/sdk/sdk_api.h"

#include "adlik_serving/server/core/server_core.h"

extern "C" void StartServer(int argc, const char** argv) {
  adlik::serving::ServerCore::inst().start(argc, argv);
}
