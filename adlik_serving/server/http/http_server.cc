#include "adlik_serving/server/http/http_server.h"

#include <map>

#include "absl/strings/str_cat.h"
#include "adlik_serving/server/http/http_rest_api_handler.h"
#include "adlik_serving/server/http/internal/http_request.h"
#include "adlik_serving/server/http/internal/http_status_code.h"
#include "adlik_serving/server/http/internal/server_interface.h"
#include "cub/base/assertions.h"
#include "cub/log/log.h"
#include "re2/re2.h"
#include "tensorflow/core/lib/core/error_codes.pb.h"

namespace adlik {
namespace serving {

namespace {

HTTPStatusCode ToHTTPStatusCode(const tensorflow::Status& status) {
  using tensorflow::error::Code;
  static const std::map<tensorflow::error::Code, HTTPStatusCode> dict = {
      {Code::OK, HTTPStatusCode::OK},
      {Code::CANCELLED, HTTPStatusCode::CLIENT_CLOSED_REQUEST},
      {Code::UNKNOWN, HTTPStatusCode::ERROR},
      {Code::INVALID_ARGUMENT, HTTPStatusCode::BAD_REQUEST},
      {Code::DEADLINE_EXCEEDED, HTTPStatusCode::GATEWAY_TO},
      {Code::NOT_FOUND, HTTPStatusCode::NOT_FOUND},
      {Code::ALREADY_EXISTS, HTTPStatusCode::CONFLICT},
      {Code::PERMISSION_DENIED, HTTPStatusCode::FORBIDDEN},
      {Code::RESOURCE_EXHAUSTED, HTTPStatusCode::TOO_MANY_REQUESTS},
      {Code::FAILED_PRECONDITION, HTTPStatusCode::BAD_REQUEST},
      {Code::ABORTED, HTTPStatusCode::CONFLICT},
      {Code::OUT_OF_RANGE, HTTPStatusCode::BAD_REQUEST},
      {Code::UNIMPLEMENTED, HTTPStatusCode::NOT_IMP},
      {Code::INTERNAL, HTTPStatusCode::ERROR},
      {Code::UNAVAILABLE, HTTPStatusCode::SERVICE_UNAV},
      {Code::DATA_LOSS, HTTPStatusCode::ERROR},
      {Code::UNAUTHENTICATED, HTTPStatusCode::UNAUTHORIZED}};
  auto it = dict.find(status.code());
  return it != dict.end() ? it->second : HTTPStatusCode::ERROR;
}

struct Dispatcher {
  Dispatcher(GetModelMetaImpl& meta_impl, PredictImpl& predict_impl, const RequestHandlerOptions& options)
      : regex_(HttpRestApiHandler::kPathRegex), handler(meta_impl, predict_impl, options) {
  }

  RequestHandler dispatch(HttpRequest* req) {
    if (RE2::FullMatch(std::string(req->uri_path()), regex_)) {
      return [this](HttpRequest* req) { this->processRequest(req); };
    }
    INFO_LOG << "Ignoring HTTP request: " << req->http_method() << " " << req->uri_path();
    return nullptr;
  }

private:
  void processRequest(HttpRequest* req) {
    std::string body;
    int64_t num_bytes = 0;
    auto request_chunk = req->readRequestBytes(&num_bytes);
    while (request_chunk != nullptr) {
      absl::StrAppend(&body, absl::string_view(request_chunk.get(), num_bytes));
      request_chunk = req->readRequestBytes(&num_bytes);
    }

    std::vector<std::pair<std::string, std::string>> headers;
    std::string output;
    INFO_LOG << "Processing HTTP request: " << req->http_method() << " " << req->uri_path();

    const auto status = handler.processRequest(req->http_method(), req->uri_path(), body, &headers, &output);

    const auto http_status = ToHTTPStatusCode(status);
    // Note: we add headers+output for non successful status too, in case the
    // output contains details about the error (e.g. error messages).
    for (const auto& kv : headers) {
      req->overwriteResponseHeader(kv.first, kv.second);
    }
    req->writeResponseString(output);
    if (http_status != HTTPStatusCode::OK) {
      INFO_LOG << "Error Processing HTTP/REST request: " << req->http_method() << " " << req->uri_path()
               << " Error: " << status.ToString();
    }
    req->replyWithStatus(http_status);
  }

  const re2::RE2 regex_;
  HttpRestApiHandler handler;
};

}  // namespace
cub::Status HttpServer::start() {
  buildServer();
  // CUB_ASSERT_VALID_PTR(server);
  if (server != nullptr) {
    if (!server->start()) {
      server.reset();
      INFO_LOG << "http server is reset...";
      return cub::Failure;
    } else {
      INFO_LOG << "http server is serving...";
      return cub::Success;
    }
  } else {
    INFO_LOG << "http server not created";
    return cub::Success;
  }
}

void HttpServer::wait() {
  if (server != nullptr) {
    server->wait();
  }
}

void HttpServer::buildServer() {
  auto http_server = build();
  if (http_server == nullptr) {
    ERR_LOG << "CreateEvHTTPServer failed";
    return;
  }

  RequestHandlerOptions handler_options = buildRequestHandlerOptions();
  std::shared_ptr<Dispatcher> dispatcher =
      std::make_shared<Dispatcher>(ROLE(GetModelMetaImpl), ROLE(PredictImpl), handler_options);
  http_server->registerRequestDispatcher([dispatcher](HttpRequest* req) { return dispatcher->dispatch(req); },
                                         handler_options);
  server = std::move(http_server);
}

}  // namespace serving
}  // namespace adlik
