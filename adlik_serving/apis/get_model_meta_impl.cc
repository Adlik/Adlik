#include "adlik_serving/apis/get_model_meta_impl.h"

#include "adlik_serving/apis/get_model_meta.pb.h"
#include "adlik_serving/framework/domain/model_store.h"
#include "adlik_serving/framework/manager/managed_store.h"
#include "tensorflow/core/lib/core/errors.h"

namespace adlik {
namespace serving {

namespace {
struct VersionStatusGetter : ManagedModelVisitor {
  VersionStatusGetter(
      const std::string& model_name,
      ::google::protobuf::Map<::google::protobuf::uint32, ::adlik::serving::ModelVersionStatus>& version_map,
      ManagedStore& store)
      : model_name(model_name), version_map(version_map), managed_store(store) {
  }

  void operator()() {
    managed_store.models(model_name, *this);
  }

  void visit(const Model& model) override {
    if (model.ready()) {
      version_map[model.getVersion()].set_ready_state(MODEL_READY);
    } else if (model.idle()) {
      version_map[model.getVersion()].set_ready_state(MODEL_UNAVAILABLE);
    } else {
      version_map[model.getVersion()].set_ready_state(MODEL_UNKNOWN);
    }
  }

  const std::string& model_name;
  ::google::protobuf::Map<::google::protobuf::uint32, ::adlik::serving::ModelVersionStatus>& version_map;
  ManagedStore& managed_store;
};
}  // namespace

tensorflow::Status GetModelMetaImpl::getModelMeta(const GetModelMetaRequest& req, GetModelMetaResponse& rsp) {
  if (auto config = ROLE(ModelStore).find(req.model_spec().name())) {
    rsp.mutable_model_spec()->CopyFrom(req.model_spec());
    rsp.mutable_config()->CopyFrom(*config);
    VersionStatusGetter(req.model_spec().name(), *rsp.mutable_version_status(), ROLE(ManagedStore))();
    return tensorflow::Status::OK();
  } else {
    return tensorflow::errors::NotFound("Not found model by name ", req.model_spec().name());
  }
}

const ModelConfig* GetModelMetaImpl::getModelConfig(const std::string& name) const {
  return ROLE(ModelStore).find(name);
}

}  // namespace serving
}  // namespace adlik
