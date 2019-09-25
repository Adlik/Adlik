#include "adlik_serving/server/grpc/grpc_server.h"

#include <map>

#include "cub/base/assertions.h"
#include "grpcpp/server.h"
#include "grpcpp/server_context.h"
#include "grpcpp/support/status.h"

namespace adlik {
namespace serving {

cub::Status GrpcServer::start() {
  if (build(builder)) {
    builder.RegisterService(this);
    server = builder.BuildAndStart();
    CUB_ASSERT_VALID_PTR(server);
    INFO_LOG << "grpc server is serving...";
  } else {
    INFO_LOG << "grpc server not started";
  }
  return cub::Success;
}

void GrpcServer::wait() {
  if (server != nullptr) {
    server->Wait();
  }
}

}  // namespace serving
}  // namespace adlik
