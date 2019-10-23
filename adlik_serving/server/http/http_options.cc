// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/http/http_options.h"

#include "adlik_serving/server/http/internal/server_interface.h"
#include "cub/cli/option_set.h"
#include "cub/cli/program_options.h"
#include "cub/log/log.h"

namespace adlik {
namespace serving {

HttpOptions::HttpOptions() : port(0), numThreads(4), timeout(30000 /* 30 seconds */) {
}

void HttpOptions::subscribe(cub::ProgramOptions& prog) {
  auto options = new cub::OptionSet{{
      cub::option("http_port",
                  &port,
                  "Port to listen on for HTTP/REST API. If set to zero "
                  "HTTP/REST API will not be exported. This port must be "
                  "different than the one specified in --port."),
      cub::option("http_num_threads",
                  &numThreads,
                  "Number of threads for HTTP/REST API processing. If not "
                  "set, will be auto set based on number of CPUs."),
      cub::option("http_timeout_in_ms", &timeout, "Timeout for HTTP/REST API calls."),
  }};
  return prog.add(options);
}

std::unique_ptr<ServerInterface> HttpOptions::build() {
  if (port > 0) {
    INFO_LOG << "http server port: " << port;
    std::unique_ptr<ServerInterface> raw;
    if (ServerInterface::create(port, numThreads, &raw)) {
      return std::move(raw);
    } else {
      ERR_LOG << "Create http server failed";
      return nullptr;
    }
  } else {
    INFO_LOG << "http port is " << port << ", can't start http server";
    return nullptr;
  }
}

RequestHandlerOptions HttpOptions::buildRequestHandlerOptions() {
  RequestHandlerOptions options;
  options.set_timeout_in_ms(timeout);
  return options;
}

}  // namespace serving
}  // namespace adlik
