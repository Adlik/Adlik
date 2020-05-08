// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/model/model_signature.h"

#include "cub/base/hash.h"

namespace tensorflow {

namespace {
template <typename Source, typename Product>
void init(const Source& src, std::set<std::string>& dst, Product f) {
  for (auto& entry : src) {
    dst.insert(f(entry));
  }
}
}  // namespace

ModelSignature::ModelSignature(const SignatureDef& def) {
  auto p = [](auto& entry) { return entry.second.name(); };
  init(def.inputs(), inputNames, p);
  init(def.outputs(), outputNames, p);
}

ModelSignature::ModelSignature(const InputTensors& ins, const OutputNames& outs) {
  init(ins, inputNames, [](auto& entry) { return entry.first; });
  init(outs, outputNames, [](auto& entry) { return entry; });
}

DEF_EQUALS(ModelSignature) {
  return FIELD_EQ(inputNames) && FIELD_EQ(outputNames);
}

std::size_t ModelSignature::hash() const noexcept {
  std::size_t seed = 0xBEEFDEAD;
  cub::hash_range(seed, inputNames);
  cub::hash_range(seed, outputNames);
  return seed;
}

std::size_t ModelSignature::getInputSize() const {
  return inputNames.size();
}

std::size_t ModelSignature::getOutputSize() const {
  return outputNames.size();
}

auto ModelSignature::getOutputs() const -> VectorNames {
  return VectorNames(outputNames.begin(), outputNames.end());
}

}  // namespace tensorflow
