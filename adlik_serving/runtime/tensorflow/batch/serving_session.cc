// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/serving_session.h"

namespace tensorflow {

namespace {
inline Status fail() {
  return errors::Unimplemented("create/extend/close was not allowed for serving session");
}
}  // namespace

Status ServingSession::Create(const GraphDef& graph) {
  return fail();
}

Status ServingSession::Extend(const GraphDef& graph) {
  return fail();
}

Status ServingSession::Close() {
  return fail();
}

Status ServingSession::Run(const InputTensors& inputs,
                           const OutputNames& names,
                           const TargetNames& targets,
                           OutputTensors* outputs) {
  RunMetadata meta;
  return Session::Run(RunOptions(), inputs, names, targets, outputs, &meta);
}

Status ServingSession::Run(const RunOptions&,
                           const InputTensors&,
                           const OutputNames&,
                           const TargetNames&,
                           OutputTensors*,
                           RunMetadata*) {
  return fail();
}

}  // namespace tensorflow
