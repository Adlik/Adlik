// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H47F34557_5F82_4DEB_9B20_AB124C5F847B
#define H47F34557_5F82_4DEB_9B20_AB124C5F847B

#include "adlik_serving/runtime/tensorflow/model/model_inputs.h"
#include "adlik_serving/runtime/tensorflow/model/model_outputs.h"
#include "tensorflow/core/public/session.h"

namespace tensorflow {

struct ServingSession : Session {
protected:
  ServingSession() = default;

private:
  Status Create(const GraphDef& graph) final;
  Status Extend(const GraphDef& graph) final;
  Status Close() final;

  Status Run(const InputTensors&, const OutputNames&, const TargetNames&, OutputTensors*) final;

  Status Run(const RunOptions&,
             const InputTensors&,
             const OutputNames&,
             const TargetNames&,
             OutputTensors*,
             RunMetadata*) override;
};

}  // namespace tensorflow

#endif
