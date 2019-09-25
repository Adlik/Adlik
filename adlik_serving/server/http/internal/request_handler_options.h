#ifndef ADLIK_SERVING_SERVER_HTTP_INTERNAL_REQUEST_HANDLER_OPTIONS_H
#define ADLIK_SERVING_SERVER_HTTP_INTERNAL_REQUEST_HANDLER_OPTIONS_H

namespace adlik {
namespace serving {

struct RequestHandlerOptions {
  inline RequestHandlerOptions& set_timeout_in_ms(int64_t timeout) {
    this->timeout = timeout;
    return *this;
  }

  inline int64_t timeout_in_ms() const {
    return timeout;
  }

private:
  int64_t timeout = 0;  // ms
};

}  // namespace serving
}  // namespace adlik

#endif
