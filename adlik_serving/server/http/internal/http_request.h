#ifndef ADLIK_SERVING_SERVER_HTTP_INTERNAL_HTTP_REQUEST_H
#define ADLIK_SERVING_SERVER_HTTP_INTERNAL_HTTP_REQUEST_H

#include <memory>

#include "absl/strings/string_view.h"
#include "adlik_serving/server/http/internal/http_status_code.h"

struct evhttp_request;

namespace adlik {
namespace serving {

struct OperationTracker;
struct EventLoopScheduler;
struct RequestHandlerOptions;

struct BlockDeleter {
public:
  BlockDeleter() : size_(0) {
  }  // nullptr
  explicit BlockDeleter(int64_t size) : size_(size) {
  }
  inline void operator()(char* ptr) const {
    // TODO: c++14 ::operator delete[](ptr, size_t)
    std::allocator<char>().deallocate(ptr, static_cast<std::size_t>(size_));
  }

private:
  int64_t size_;
};

struct HttpRequest {
  static bool create(evhttp_request* ev_request,
                     OperationTracker& op_tracker,
                     EventLoopScheduler& scheduler,
                     std::unique_ptr<HttpRequest>*);

  virtual ~HttpRequest() = default;

  virtual absl::string_view uri_path() const = 0;

  virtual absl::string_view http_method() const = 0;

  virtual std::unique_ptr<char[], BlockDeleter> readRequestBytes(int64_t* size) = 0;

  virtual void overwriteResponseHeader(absl::string_view header, absl::string_view value) = 0;

  virtual void writeResponseString(absl::string_view data) = 0;

  virtual void writeResponseBytes(const char* data, int64_t size) = 0;

  virtual void replyWithStatus(HTTPStatusCode status) = 0;

  virtual void SetHandlerOptions(const RequestHandlerOptions& handler_options) = 0;
};

}  // namespace serving
}  // namespace adlik

#endif
