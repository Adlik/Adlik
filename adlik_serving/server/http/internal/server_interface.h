// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_SERVER_HTTP_INTERNAL_SERVER_INTERFACE_H
#define ADLIK_SERVING_SERVER_HTTP_INTERNAL_SERVER_INTERFACE_H

#include <functional>
#include <memory>

namespace adlik {
namespace serving {

struct HttpRequest;
struct RequestHandlerOptions;

typedef std::function<void(HttpRequest*)> RequestHandler;
typedef std::function<RequestHandler(HttpRequest*)> RequestDispatcher;

struct ServerInterface {
  static bool create(int port, int num_threads, std::unique_ptr<ServerInterface>*);

  virtual ~ServerInterface() = default;
  virtual bool start() = 0;
  virtual void wait() = 0;
  virtual void registerRequestDispatcher(RequestDispatcher dispatcher, const RequestHandlerOptions& options) = 0;
};

}  // namespace serving
}  // namespace adlik

#endif
