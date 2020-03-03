// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/core/server_core.h"

#include "adlik_serving/apis/get_model_meta_impl.h"
#include "adlik_serving/apis/predict_impl.h"
#include "adlik_serving/apis/task_op_impl.h"
#include "adlik_serving/framework/domain/event_bus.h"
#include "adlik_serving/framework/domain/model_options.h"
#include "adlik_serving/framework/domain/model_store.h"
#include "adlik_serving/framework/domain/state_monitor.h"
#include "adlik_serving/framework/manager/boarding_loop.h"
#include "adlik_serving/framework/manager/managed_store.h"
#include "adlik_serving/framework/manager/model_factory_suite.h"
#include "adlik_serving/framework/manager/runtime.h"
#include "adlik_serving/framework/manager/runtime_context.h"
#include "adlik_serving/framework/manager/runtime_suite.h"
#include "adlik_serving/framework/manager/serving_store.h"
#include "adlik_serving/framework/manager/storage_loop.h"
#include "adlik_serving/server/grpc/grpc_server.h"
#include "adlik_serving/server/http/http_server.h"
#include "cub/base/assertions.h"
#include "cub/base/status.h"
#include "cub/cli/program_options.h"

namespace adlik {
namespace serving {

struct ServerCoreImpl : private ModelOptions,
                        private StorageLoop,
                        private BoardingLoop,
                        private ModelStore,
                        private ManagedStore,
                        private ServingStore,
                        private ModelFactorySuite,
                        private StateMonitor,
                        private EventBus,
                        private RuntimeContext,
                        private PredictImpl,
                        private GetModelMetaImpl,
                        private TaskOpImpl,
                        private GrpcServer,
                        private HttpServer,
                        ServerCore {
private:
  OVERRIDE(cub::Status start(int argc, const char** argv)) {
    INFO_LOG << "Adlik serving is initializing..." << std::endl;
    CUB_ASSERT_SUCC_CALL(init(argc, argv));
    CUB_ASSERT_SUCC_CALL(config());

    INFO_LOG << "Adlik serving is running..." << std::endl;
    CUB_ASSERT_SUCC_CALL(start());

    wait();

    return cub::Success;
  }

public:
  cub::Status spredict(const PredictRequest& req, PredictResponse& rsp) {
    auto status = ROLE(PredictImpl).predict(req, rsp);
    return cub::Status(status.code());
  }

private:
  cub::Status init(int argc, const char** argv) {
    cub::ProgramOptions prog;

    // framework options.
    SELF(GrpcServer).subscribe(prog);
    SELF(HttpServer).subscribe(prog);
    SELF(ModelOptions).subscribe(prog);

    // runtime options.
    Runtime& runtime = RuntimeSuite::inst();
    runtime.init(prog);

    return prog.parse(argc, argv);
  }

  cub::Status config() {
    // configure runtime.
    Runtime& runtime = RuntimeSuite::inst();
    CUB_ASSERT_SUCC_CALL(runtime.config(SELF(RuntimeContext)));

    // configure model store.
    CUB_ASSERT_SUCC_CALL(SELF(ModelStore).config());
    INFO_LOG << "Config ModelStore success";

    // connect roles.
    SELF(BoardingLoop).poll();
    SELF(StateMonitor).connect(SELF(EventBus));
    connect(SELF(StorageLoop), SELF(BoardingLoop));

    INFO_LOG << "Connect Model success";

    // waiting all models loaded.
    SELF(StateMonitor).wait();
    return cub::Success;
  }

  cub::Status start() {
    auto status = SELF(GrpcServer).start();
    if (cub::isSuccStatus(status)) {
      return SELF(HttpServer).start();
    } else {
      return status;
    }
  }

  void wait() {
    SELF(HttpServer).wait();
    SELF(GrpcServer).wait();
  }

  void connect(ModelSource& src, ModelTarget& target) {
    src.connect(target);
  }

private:
  IMPL_ROLE(ModelOptions)
  IMPL_ROLE(ModelStore)
  IMPL_ROLE(ManagedStore)
  IMPL_ROLE(ServingStore)
  IMPL_ROLE(ModelFactory)
  IMPL_ROLE(ModelFactorySuite)
  IMPL_ROLE(EventBus)
  IMPL_ROLE(PredictImpl)
  IMPL_ROLE(GetModelMetaImpl)
  IMPL_ROLE(TaskOpImpl)
};

ServerCore& ServerCore::inst() {
  static ServerCoreImpl inst;
  return inst;
}

cub::Status start(int argc, const char** argv) {
  return ServerCore::inst().start(argc, argv);
}

cub::Status spredict(const PredictRequest& req, PredictResponse& rsp) {
  ServerCoreImpl& core = static_cast<ServerCoreImpl&>(ServerCore::inst());
  return core.spredict(req, rsp);
}

}  // namespace serving
}  // namespace adlik
