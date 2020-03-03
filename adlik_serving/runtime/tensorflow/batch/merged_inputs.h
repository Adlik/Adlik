// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HA65BB825_A944_4634_AE53_FC4EE2BEF414
#define HA65BB825_A944_4634_AE53_FC4EE2BEF414

#include <map>

#include "adlik_serving/runtime/tensorflow/batch/final_padding.h"
#include "adlik_serving/runtime/tensorflow/batch/merged_tensors.h"
#include "adlik_serving/runtime/tensorflow/model/model_inputs.h"

namespace tensorflow {

struct ModelSignature;
struct MergedDimensions;

struct MergedInputs {
  MergedInputs(bool padding, const MergedDimensions&);

  Status append(const std::string&, const Tensor&, const FinalPadding&);
  Status merge(const ModelSignature&, InputTensors&) const;

private:
  Status doPadding(const std::string&, const Tensor&, Tensor&) const;
  Status padding(const std::string&, const Tensor&, Tensor&) const;
  Status concat(const std::string&, Tensor&) const;
  Status merge(const std::string&, InputTensors&) const;

private:
  bool shouldPadding;
  const MergedDimensions& dims;
  std::map<std::string, MergedTensors> tensors;
};

}  // namespace tensorflow

#endif
