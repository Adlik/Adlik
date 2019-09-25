#include "adlik_serving/runtime/tensorflow/batch/batching_session.h"
#include "adlik_serving/runtime/tensorflow/batch/batching_scheduler.h"
#include "adlik_serving/runtime/tensorflow/batch/inferential_batch.h"
#include "adlik_serving/runtime/tensorflow/model/meta_graph.h"

namespace tensorflow {

BatchingSession::BatchingSession(Session& session) : session(session) {
}

void BatchingSession::config() {
  auto createQueue = [this](auto& signature) {
    UniqueBatcher batcher;
    this->ROLE(BatchingScheduler).append(signature, *this, batcher);
    schedulers[signature] = std::move(batcher);
  };
  ROLE(MetaGraph).signatures(createQueue);
}

// task has bee moved into batcher after invoke schedule.
template <typename... Params>
inline Status BatchingSession::batch(UniqueBatcher& batcher, Params&&... params) {
  auto task = std::make_unique<InferentialTask>(std::forward<Params>(params)...);
  auto done = task.get();
  TF_RETURN_IF_ERROR(batcher->Schedule(&task));
  return done->wait();
}

Status BatchingSession::Run(const RunOptions& opts,
                            const InputTensors& inputs,
                            const OutputNames& names,
                            const TargetNames& targets,
                            OutputTensors* outputs,
                            RunMetadata* meta) {
  auto it = schedulers.find(ModelSignature(inputs, names));
  if (it != schedulers.end()) {
    return batch(it->second, opts, inputs, names, *outputs, *meta);
  } else {
    return session.Run(opts, inputs, names, targets, outputs, meta);
  }
}

Status BatchingSession::ListDevices(std::vector<DeviceAttributes>* rsp) {
  return session.ListDevices(rsp);
}

void BatchingSession::process(const ModelSignature& signature, const InferentialBatch& batch) {
  batch.process(signature, session, ROLE(BatchingParameters));
}

}  // namespace tensorflow
