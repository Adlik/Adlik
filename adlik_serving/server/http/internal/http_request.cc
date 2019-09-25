#include "adlik_serving/server/http/internal/http_request.h"

#include <map>

#include "absl/types/span.h"
#include "adlik_serving/server/http/internal/event_loop_scheduler.h"
#include "adlik_serving/server/http/internal/operation_tracker.h"
#include "adlik_serving/server/http/internal/request_handler_options.h"
#include "cub/base/uncloneable.h"
#include "cub/log/log.h"
#include "libevent/include/event2/buffer.h"
#include "libevent/include/event2/http.h"

namespace adlik {
namespace serving {

namespace {

constexpr char CONTENT_ENCODING[] = "Content-Encoding";

const char* cmdToMethod(evhttp_cmd_type cmd_type) {
  static const std::map<evhttp_cmd_type, const char*> dict = {{EVHTTP_REQ_GET, "GET"},
                                                              {EVHTTP_REQ_POST, "POST"},
                                                              {EVHTTP_REQ_HEAD, "HEAD"},
                                                              {EVHTTP_REQ_PUT, "PUT"},
                                                              {EVHTTP_REQ_DELETE, "DELETE"},
                                                              {EVHTTP_REQ_OPTIONS, "OPTIONS"},
                                                              {EVHTTP_REQ_TRACE, "TRACE"},
                                                              {EVHTTP_REQ_CONNECT, "CONNECT"},
                                                              {EVHTTP_REQ_PATCH, "PATCH"}};
  auto it = dict.find(cmd_type);
  return it != dict.end() ? it->second : nullptr;
}

struct RequestImpl : HttpRequest {
  RequestImpl(evhttp_request* ev_request, OperationTracker&, EventLoopScheduler& scheduler);

  ~RequestImpl();

  bool init();

  absl::string_view uri_path() const override;

  absl::string_view http_method() const override;

  std::unique_ptr<char[], BlockDeleter> readRequestBytes(int64_t* size) override;

  void SetHandlerOptions(const RequestHandlerOptions& handler_options) override {
    this->handler_options_ = &handler_options;
  }

  void overwriteResponseHeader(absl::string_view header, absl::string_view value) override;

  void writeResponseString(absl::string_view data) override;

  void writeResponseBytes(const char* data, int64_t size) override;

  void replyWithStatus(HTTPStatusCode status) override;

private:
  DISALLOW_COPY_AND_ASSIGN(RequestImpl);

  void abort();

  void evSendReply(HTTPStatusCode status);

  evhttp_request* ev_request;
  OperationTracker& op_tracker;  // not owned
  EventLoopScheduler& scheduler;  // not owned

  const char* method = nullptr;
  const char* uri = nullptr;
  evhttp_uri* decoded_uri = nullptr;
  const char* path = nullptr;
  evkeyvalq* headers = nullptr;
  evbuffer* output_buf = nullptr;  // owned by this
  std::string path_and_query;
  const RequestHandlerOptions* handler_options_ = nullptr;
};

RequestImpl::RequestImpl(evhttp_request* ev_request, OperationTracker& op_tracker, EventLoopScheduler& scheduler)
    : ev_request(ev_request), op_tracker(op_tracker), scheduler(scheduler) {
}

RequestImpl::~RequestImpl() {
  if (output_buf != nullptr) {
    evbuffer_free(output_buf);
  }
}

bool RequestImpl::init() {
  method = cmdToMethod(evhttp_request_get_command(ev_request));
  if (method == nullptr) {
    return false;
  }
  uri = evhttp_request_get_uri(ev_request);
  decoded_uri = evhttp_uri_parse(uri);
  if (decoded_uri == nullptr) {
    return false;
  }

  // NB: need double-check "/" is OK
  path = evhttp_uri_get_path(decoded_uri);
  if (path == nullptr) {
    path = "/";
  }

  path_and_query = path;
  const char* query = evhttp_uri_get_query(decoded_uri);
  if (query != nullptr) {
    path_and_query.push_back('?');
    path_and_query.append(query);
  }

  const char* fragment = evhttp_uri_get_fragment(decoded_uri);
  if (fragment != nullptr) {
    path_and_query.push_back('#');
    path_and_query.append(fragment);
  }
  headers = evhttp_request_get_input_headers(ev_request);
  output_buf = evbuffer_new();
  return output_buf != nullptr;
}

absl::string_view RequestImpl::uri_path() const {
  return path_and_query;
}

absl::string_view RequestImpl::http_method() const {
  return method;
}

std::unique_ptr<char[], BlockDeleter> RequestImpl::readRequestBytes(int64_t* size) {
  evbuffer* input_buf = evhttp_request_get_input_buffer(ev_request);
  if (input_buf == nullptr) {
    *size = 0;
    return nullptr;  // no body
  }

  // possible a reentry after gzip uncompression
  if (evbuffer_get_length(input_buf) == 0) {
    *size = 0;
    return nullptr;  // EOF
  }

  auto buf_size = reinterpret_cast<size_t*>(size);
  *buf_size = evbuffer_get_contiguous_space(input_buf);
  assert(*buf_size > 0);

  char* block = std::allocator<char>().allocate(*buf_size);
  int ret = evbuffer_remove(input_buf, block, *buf_size);

  if ((size_t)ret != *buf_size) {
    ERR_LOG << "Unexpected: read less than specified num_bytes : " << buf_size;
    std::allocator<char>().deallocate(block, *buf_size);
    *buf_size = 0;
    return nullptr;  // don't return corrupted buffer
  }

  return std::unique_ptr<char[], BlockDeleter>(block, BlockDeleter(*buf_size));
}

void RequestImpl::overwriteResponseHeader(absl::string_view header, absl::string_view value) {
  evkeyvalq* ev_headers = evhttp_request_get_output_headers(ev_request);

  std::string header_str = std::string(header.data(), header.size());
  const char* header_cstr = header_str.c_str();

  evhttp_remove_header(ev_headers, header_cstr);
  evhttp_add_header(ev_headers, header_cstr, std::string(value.data(), value.size()).c_str());
}

void RequestImpl::writeResponseString(absl::string_view data) {
  writeResponseBytes(data.data(), static_cast<int64_t>(data.size()));
}

void RequestImpl::writeResponseBytes(const char* data, int64_t size) {
  assert(size >= 0);
  if (output_buf == nullptr) {
    FATAL_LOG << "Request not initialized.";
    return;
  }

  int ret = evbuffer_add(output_buf, data, static_cast<size_t>(size));
  if (ret == -1) {
    ERR_LOG << "Failed to write " << static_cast<size_t>(size) << " bytes data to output buffer";
  }
}

void RequestImpl::replyWithStatus(HTTPStatusCode status) {
  bool result = scheduler.schedule([this, status]() { evSendReply(status); });
  if (!result) {
    ERR_LOG << "Failed to EventLoopSchedule replyWithStatus()";
    abort();
  }
}

void RequestImpl::evSendReply(HTTPStatusCode status) {
  evhttp_send_reply(ev_request, static_cast<int>(status), nullptr, output_buf);
  op_tracker.decOps();
  delete this;
}

void RequestImpl::abort() {
  evhttp_send_error(ev_request, HTTP_INTERNAL, nullptr);
  op_tracker.decOps();
  delete this;
}

}  // namespace

bool HttpRequest::create(evhttp_request* ev_request,
                         OperationTracker& op_tracker,
                         EventLoopScheduler& scheduler,
                         std::unique_ptr<HttpRequest>* request) {
  auto raw_request = std::make_unique<RequestImpl>(ev_request, op_tracker, scheduler);
  if (raw_request->init()) {
    *request = std::move(raw_request);
    return true;
  }
  return false;
}

}  // namespace serving
}  // namespace adlik
