#ifndef ADLIK_SERVING_SERVER_HTTP_HTTP_OPTIONS_H
#define ADLIK_SERVING_SERVER_HTTP_HTTP_OPTIONS_H

#include <memory>

#include "adlik_serving/server/http/internal/request_handler_options.h"
#include "cub/cli/option_subscriber.h"

namespace adlik {
namespace serving {

struct ServerInterface;

struct HttpOptions : cub::OptionSubscriber {
  HttpOptions();

  OVERRIDE(void subscribe(cub::ProgramOptions&));

  std::unique_ptr<ServerInterface> build();

  RequestHandlerOptions buildRequestHandlerOptions();

private:
  int port;
  int numThreads;
  int timeout;  // ms
};

}  // namespace serving
}  // namespace adlik

#endif
