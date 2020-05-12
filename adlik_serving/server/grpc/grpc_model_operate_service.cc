#include "adlik_serving/server/grpc/grpc_model_operate_service.h"

#include "adlik_serving/apis/model_operate_impl.h"
#include "grpc/grpc.h"

namespace adlik {

namespace {
::grpc::Status toGrpcStatus(const tensorflow::Status& status) {
  return {static_cast<grpc::StatusCode>(status.code()), status.error_message()};
}
}  // namespace
namespace serving {
::grpc::Status GrpcModelOperateService::addModel(::grpc::ServerContext* ctxt,
                                                 const ModelOperateRequest* req,
                                                 ModelOperateResponse* rsp) {
  auto status = ROLE(ModelOperateImpl).addModel(*req, *rsp);
  return toGrpcStatus(status);
}

::grpc::Status GrpcModelOperateService::deleteModel(::grpc::ServerContext* ctxt,
                                                    const ModelOperateRequest* req,
                                                    ModelOperateResponse* rsp) {
  auto status = ROLE(ModelOperateImpl).deleteModel(*req, *rsp);
  return toGrpcStatus(status);
}
}  // namespace serving
}  // namespace adlik