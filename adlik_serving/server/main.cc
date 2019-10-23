// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/core/server_core.h"
#include "adlik_serving/server/core/version.h"
#include "cub/base/assertions.h"

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int sig) {
  void* array[10];

  // get void*'s for all entries on the stack
  auto size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

using namespace adlik::serving;

int main(int argc, const char** argv) {
  INFO_LOG << "Adlik serving version: " << version();
  signal(SIGABRT, handler);
  return ServerCore::inst().start(argc, argv);
}
