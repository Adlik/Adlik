// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/http/internal/server_interface.h"

#include <signal.h>

#include <vector>

#include "absl/base/call_once.h"
#include "absl/base/internal/raw_logging.h"
#include "absl/memory/memory.h"
#include "absl/synchronization/notification.h"
#include "adlik_serving/server/http/internal/event_executor.h"
#include "adlik_serving/server/http/internal/event_loop_scheduler.h"
#include "adlik_serving/server/http/internal/http_request.h"
#include "adlik_serving/server/http/internal/operation_tracker.h"
#include "adlik_serving/server/http/internal/request_handler_options.h"
#include "cub/base/uncloneable.h"
#include "cub/log/log.h"
#include "libevent/include/event2/event.h"
#include "libevent/include/event2/http.h"
#include "libevent/include/event2/thread.h"
#include "libevent/include/event2/util.h"

namespace adlik {
namespace serving {

namespace {

absl::once_flag libevent_init_once;

void InitLibEvent() {
  if (evthread_use_pthreads() != 0) {
    FATAL_LOG << "Http server requires pthread support.";
  }
  signal(SIGPIPE, SIG_IGN);
}

void GlobalInitialize() {
  absl::call_once(libevent_init_once, &InitLibEvent);
}

void EvImmediateCallback(evutil_socket_t socket, int16_t flags, void* arg) {
  auto fn = static_cast<std::function<void()>*>(arg);
  (*fn)();
  delete fn;
}

struct ServerImpl : ServerInterface, EventLoopScheduler {
  ServerImpl(int port, int num_threads);
  ~ServerImpl();

  bool init();
  bool start() override;
  void wait() override;
  void registerRequestDispatcher(RequestDispatcher dispatcher, const RequestHandlerOptions& options) override;

  bool schedule(std::function<void()> fn) override;

private:
  DISALLOW_COPY_AND_ASSIGN(ServerImpl);

  static void dispatchEvRequestFn(struct evhttp_request* req, void* server);

  void dispatchEvRequest(evhttp_request* req);
  void scheduleHandler(RequestHandler&& handler, HttpRequest* request);
  void terminate();
  bool isTerminating() const;
  bool isRunning() const;

  int port;
  int num_threads;
  std::unique_ptr<EventExecutor> executor;  // own this executor
  OperationTracker tracker;

  // ev objs
  event_base* ev_base = nullptr;
  evhttp* ev_http = nullptr;
  evhttp_bound_socket* ev_listener = nullptr;
  const timeval* immediate = nullptr;

  absl::Notification started;
  absl::Notification terminating;

  struct DispatcherInfo {
    DispatcherInfo(RequestDispatcher dispatcher_in, const RequestHandlerOptions& options_in)
        : dispatcher(std::move(dispatcher_in)), options(options_in) {
    }

    const RequestDispatcher dispatcher;
    const RequestHandlerOptions options;
  };

  mutable absl::Mutex request_mu;
  std::vector<DispatcherInfo> dispatchers GUARDED_BY(request_mu);
};

ServerImpl::ServerImpl(int port, int num_threads) : port(port), num_threads(num_threads) {
}

ServerImpl::~ServerImpl() {
  if (!isTerminating()) {
    ERR_LOG << "Serer has not been terminated. Force termination now.";
    terminate();
  }

  if (ev_http != nullptr) {
    // this frees the socket handlers too
    evhttp_free(ev_http);
  }

  if (ev_base != nullptr) {
    event_base_free(ev_base);
  }
}

bool ServerImpl::init() {
  if (port <= 0) {
    ERR_LOG << "Http server port is " << port << ", not specified.";
    return false;
  }

  executor = EventExecutor::create(num_threads);

  // Must do this before event_base_new
  GlobalInitialize();

  // This ev_base created per-server v.s. global
  ev_base = event_base_new();
  if (ev_base == nullptr) {
    ERR_LOG << "Failed to create an event_base.";
    return false;
  }

  timeval tv_zero = {0, 0};
  immediate = event_base_init_common_timeout(ev_base, &tv_zero);

  ev_http = evhttp_new(ev_base);
  if (ev_http == nullptr) {
    ERR_LOG << "Failed to create evhttp.";
    return false;
  }

  evhttp_set_gencb(ev_http, &dispatchEvRequestFn, this);
  return true;
}

bool ServerImpl::start() {
  // "::"  =>  in6addr_any
  ev_uint16_t ev_port = static_cast<ev_uint16_t>(port);
  ev_listener = evhttp_bind_socket_with_handle(ev_http, "::", ev_port);
  if (ev_listener == nullptr) {
    // in case ipv6 is not supported, fallback to inaddr_any
    ev_listener = evhttp_bind_socket_with_handle(ev_http, nullptr, ev_port);
    if (ev_listener == nullptr) {
      ERR_LOG << "Couldn't bind to port " << port;
      return false;
    }
  }

  // Listener counts as an active operation
  tracker.incOps();
  tracker.incOps();

  executor->schedule([this]() {
    INFO_LOG << "Entering the event loop ...";
    int result = event_base_dispatch(ev_base);
    INFO_LOG << "event_base_dispatch() exits with value " << result;

    tracker.decOps();
  });

  started.Notify();
  return true;
}

void ServerImpl::wait() {
  tracker.waitToOne();

  int result = event_base_loopexit(ev_base, nullptr);
  INFO_LOG << "event_base_loopexit() exits with value " << result;

  tracker.waitDone();
}

void ServerImpl::registerRequestDispatcher(RequestDispatcher dispatcher, const RequestHandlerOptions& options) {
  absl::MutexLock l(&request_mu);
  dispatchers.emplace_back(dispatcher, options);
}

bool ServerImpl::schedule(std::function<void()> fn) {
  auto scheduled_fn = new std::function<void()>(std::move(fn));
  int result =
      event_base_once(ev_base, -1, EV_TIMEOUT, EvImmediateCallback, static_cast<void*>(scheduled_fn), immediate);
  return result == 0;
}

void ServerImpl::dispatchEvRequestFn(struct evhttp_request* req, void* server) {
  ServerImpl* impl = static_cast<ServerImpl*>(server);
  impl->dispatchEvRequest(req);
}

void ServerImpl::dispatchEvRequest(evhttp_request* req) {
  std::unique_ptr<HttpRequest> request;
  if (!HttpRequest::create(req, tracker, *this, &request)) {
    evhttp_send_error(req, HTTP_BADREQUEST, nullptr);
    return;
  }

  bool dispatched = false;
  {
    absl::MutexLock l(&request_mu);

    for (const auto& dispatcher : dispatchers) {
      auto handler = dispatcher.dispatcher(request.get());
      if (handler == nullptr) {
        continue;
      }
      request->SetHandlerOptions(dispatcher.options);
      tracker.incOps();
      dispatched = true;
      scheduleHandler(std::move(handler), request.release());
      break;
    }
  }

  if (!dispatched) {
    evhttp_send_error(req, HTTP_NOTFOUND, nullptr);
    return;
  }
}

void ServerImpl::scheduleHandler(RequestHandler&& handler, HttpRequest* request) {
  executor->schedule([handler, request]() { handler(request); });
}

void ServerImpl::terminate() {
  if (!isRunning()) {
    INFO_LOG << "Server is not running ...";
    return;
  }

  if (isTerminating()) {
    ERR_LOG << "Server is already being terminated ...";
    return;
  }

  terminating.Notify();

  this->schedule([this]() {
    evhttp_del_accept_socket(ev_http, ev_listener);
    tracker.decOps();
  });
}

bool ServerImpl::isTerminating() const {
  return terminating.HasBeenNotified();
}

bool ServerImpl::isRunning() const {
  return started.HasBeenNotified();
}

}  // namespace

bool ServerInterface::create(int port, int num_threads, std::unique_ptr<ServerInterface>* server) {
  auto raw = std::make_unique<ServerImpl>(port, num_threads);
  if (raw->init()) {
    *server = std::move(raw);
    return true;
  }
  ERR_LOG << "Create http server impl failed";
  return false;
}

}  // namespace serving
}  // namespace adlik
