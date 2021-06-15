// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/factory/tf_runtime.h"

#include "adlik_serving/apis/predict.pb.h"
#include "adlik_serving/framework/domain/auto_model_handle.h"
#include "adlik_serving/framework/domain/model_id.h"
#include "adlik_serving/framework/manager/auto_runtime_register.h"
#include "adlik_serving/framework/manager/model_factory_suite.h"
#include "adlik_serving/framework/manager/run_options.h"
#include "adlik_serving/framework/manager/runtime_context.h"
#include "adlik_serving/framework/manager/serving_store.h"
#include "adlik_serving/runtime/tensorflow/model/model_inputs.h"
#include "adlik_serving/runtime/tensorflow/model/model_outputs.h"
#include "adlik_serving/runtime/tensorflow/model/plan_model.h"
#include "cub/base/assertions.h"
#include "cub/base/optional.h"
#include "tensorflow/core/protobuf/meta_graph.pb.h"
#include "tensorflow/core/public/session.h"

namespace tensorflow {

namespace {
constexpr char DefaultSignatureKey[] = "serving_default";
constexpr char PredictMethodName[] = "tensorflow/serving/predict";

inline Status fail(const std::string& msg = "") {
  return errors::InvalidArgument(msg);
}

struct Signature {
  Signature(const SignatureDef& def) : def(def) {
  }

  template <typename F>
  Status input(const std::string& alias, F f) const {
    return io(def.inputs(), alias, f);
  }

  template <typename F>
  Status output(const std::string& alias, F f) const {
    return io(def.outputs(), alias, f);
  }

  template <typename F>
  void outputs(F f) const {
    for (auto& output : def.outputs()) {
      f(output);
    }
  }

private:
  template <typename Src, typename F>
  static Status io(const Src& src, const std::string& alias, F f) {
    auto iter = src.find(alias);
    return iter != src.end() ? f(iter->second.name()) : fail();
  }

private:
  const SignatureDef& def;
};

struct PlanModelPredictor {
  PlanModelPredictor(const RunOptions& opts,
                     const adlik::serving::ModelId& id,
                     const adlik::serving::PredictRequest& req,
                     adlik::serving::PredictResponse& rsp,
                     PlanModel& bundle)
      : opts(opts), id(id), req(req), rsp(rsp), graph(bundle.ROLE(MetaGraphDef)), session(bundle.ROLE(Session)) {
  }

  Status predict() {
    if (auto signature = getSignature()) {
      fillModelSpec(*rsp.mutable_model_spec());
      return doPredict(*signature);
    } else {
      return fail();
    }
  }

private:
  std::string getSignatureName() {
    auto& name = req.model_spec().signature_name();
    return !name.empty() ? name : DefaultSignatureKey;
  }

  cub::Optional<SignatureDef> getSignature() {
    auto iter = graph.signature_def().find(getSignatureName());
    if (iter != graph.signature_def().end()) {
      return iter->second;
    } else {
      return cub::nilopt;
    }
  }

  Status doPredict(const SignatureDef& signature) {
    InputTensors inputs;
    OutputNames names;
    OutputAliases aliases;
    OutputTensors outputs;
    RunMetadata meta;
    TF_RETURN_IF_ERROR(preprocess(signature, inputs, names, aliases));
    TF_RETURN_IF_ERROR(session.Run(opts, inputs, names, {}, &outputs, &meta));
    TF_RETURN_IF_ERROR(postprocess(aliases, outputs));
    return Status::OK();
  }

  void fillModelSpec(adlik::serving::ModelSpec& spec) {
    spec.set_name(req.model_spec().name());
    spec.set_signature_name(getSignatureName());
    spec.mutable_version()->set_value(id.getVersion());
  }

  template <typename Pred>
  static Status verify(Pred pred) {
    return pred() ? Status::OK() : fail();
  }

  Status fillInputTensor(const Signature& signature,
                         const std::string& alias,
                         const TensorProto& proto,
                         InputTensors& inputs) {
    return signature.input(alias, [&inputs, &proto](auto& name) {
      Tensor tensor;
      if (tensor.FromProto(proto)) {
        inputs.emplace_back(name, tensor);
      }
      return Status::OK();
    });
  }

  Status fillInputTensors(const Signature& signature, InputTensors& inputs) {
    for (auto& i : req.inputs()) {
      TF_RETURN_IF_ERROR(fillInputTensor(signature, i.first, i.second, inputs));
    }
    return Status::OK();
  }

  Status fillOneName(const Signature& signature,
                     const std::string& alias,
                     std::set<std::string> seens,
                     OutputNames& names,
                     OutputAliases& aliases) {
    auto fill = [&alias, &seens, &names, &aliases](auto& name) {
      if (seens.find(alias) != seens.end()) {
        return fail();
      }
      seens.insert(alias);
      names.emplace_back(name);
      aliases.emplace_back(alias);
      return Status::OK();
    };
    return signature.output(alias, fill);
  }

  Status fillNamesByFilters(const Signature& signature, OutputNames& names, OutputAliases& aliases) {
    std::set<std::string> seens;
    for (auto& it : req.output_filter()) {
      TF_RETURN_IF_ERROR(fillOneName(signature, it.first, seens, names, aliases));
    }
    return Status::OK();
  }

  void fillNamesByOutputs(const Signature& signature, OutputNames& names, OutputAliases& aliases) {
    signature.outputs([&names, &aliases](auto& p) {
      names.emplace_back(p.second.name());
      aliases.emplace_back(p.first);
    });
  }

  Status fillOutputNames(const Signature& signature, OutputNames& names, OutputAliases& aliases) {
    TF_RETURN_IF_ERROR(fillNamesByFilters(signature, names, aliases));
    if (names.empty()) {
      fillNamesByOutputs(signature, names, aliases);
    }
    return Status::OK();
  }

  static Status verifySignatureName(const SignatureDef& def) {
    return verify([&def]() { return def.method_name() == PredictMethodName; });
  }

  Status verifyInputsSize(const SignatureDef& def) {
    return verify([this, &def]() { return req.inputs().size() == def.inputs().size(); });
  }

  Status preprocess(const SignatureDef& signature, InputTensors& inputs, OutputNames& names, OutputAliases& aliases) {
    TF_RETURN_IF_ERROR(verifySignatureName(signature));
    TF_RETURN_IF_ERROR(verifyInputsSize(signature));
    TF_RETURN_IF_ERROR(fillInputTensors(signature, inputs));
    TF_RETURN_IF_ERROR(fillOutputNames(signature, names, aliases));
    return Status::OK();
  }

  static Status verifyOutputSize(const OutputAliases& aliases, const OutputTensors& outputs) {
    return verify([&aliases, &outputs]() { return outputs.size() == aliases.size(); });
  }

  void fillOutput(const Tensor& tensor, const std::string& alias) {
    auto& proto = *((*rsp.mutable_outputs())[alias].mutable_tensor());
    // tensor.AsProtoField(&proto);
    tensor.AsProtoTensorContent(&proto);
  }

  Status postprocess(const OutputAliases& aliases, const OutputTensors& outputs) {
    TF_RETURN_IF_ERROR(verifyOutputSize(aliases, outputs));
    for (decltype(outputs.size()) i = 0; i != outputs.size(); ++i) {
      fillOutput(outputs[i], aliases[i]);
    }
    return Status::OK();
  }

private:
  const RunOptions& opts;
  const adlik::serving::ModelId& id;
  const adlik::serving::PredictRequest& req;
  adlik::serving::PredictResponse& rsp;
  MetaGraphDef& graph;
  Session& session;
};

RunOptions toTfOptions(const adlik::serving::RunOptions& opts) {
  RunOptions tf_options;
  tf_options.set_timeout_in_ms(opts.timeout_in_ms);
  return tf_options;
}

}  // namespace

REGISTER_RUNTIME(TfRuntime, "tensorflow");
REGISTER_RUNTIME(TfRuntime, "tensorflow-tensorrt");

TfRuntime::TfRuntime(const std::string& name) : runtime_name(name) {
}

void TfRuntime::init(cub::ProgramOptions& prog) {
  INFO_LOG << "initialize tensorflow runtime";
  SELF(TfPlanModelOptions).subscribe(prog);
}

cub::Status TfRuntime::config(const adlik::serving::RuntimeContext& ctxt) {
  INFO_LOG << "configure tensorflow runtime";

  SELF(TfPlanModelConfig).config(SELF(TfPlanModelOptions));
  SELF(BatchingParameters).config(SELF(TfPlanModelConfig));

  CUB_ASSERT_TRUE(SELF(BatchingScheduler).config().ok());

  ctxt.ROLE(ModelFactorySuite).add(runtime_name, *this);

  return cub::Success;
}

Status TfRuntime::predict(const adlik::serving::RunOptions& opts,
                          adlik::serving::ModelHandle* handle,
                          const adlik::serving::PredictRequest& req,
                          adlik::serving::PredictResponse& rsp) {
  adlik::serving::AutoModelHandle<PlanModel> bundle(handle);
  return PlanModelPredictor(toTfOptions(opts), bundle.id(), req, rsp, *bundle).predict();
}

}  // namespace tensorflow
