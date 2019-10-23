// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HB639DB43_6FB9_4A84_84F1_3537FABB0281
#define HB639DB43_6FB9_4A84_84F1_3537FABB0281

#include <set>

#include "adlik_serving/runtime/tensorflow/model/model_inputs.h"
#include "adlik_serving/runtime/tensorflow/model/model_outputs.h"
#include "cub/base/comparator.h"
#include "tensorflow/core/protobuf/meta_graph.pb.h"

namespace tensorflow {

struct ModelSignature {
  using TensorNames = std::set<std::string>;
  using VectorNames = std::vector<std::string>;

  ModelSignature(const SignatureDef&);
  ModelSignature(const InputTensors&, const OutputNames&);

  DECL_EQUALS(ModelSignature);

  std::size_t hash() const noexcept;

  std::size_t getInputSize() const;
  std::size_t getOutputSize() const;

  template <typename F>
  Status inputs(F f) const {
    return foreach (inputNames, f);
  }

  template <typename F>
  Status outputs(F f) const {
    return foreach (outputNames, f);
  }

  VectorNames getOutputs() const;

private:
  template <typename F>
  static Status foreach (const TensorNames& names, F f) {
    for (auto& name : names) {
      if (!f(name).ok())
        return errors::Internal("callback failed");
    }
    return Status::OK();
  }

private:
  TensorNames inputNames;
  TensorNames outputNames;
};

}  // namespace tensorflow

namespace std {
template <>
struct hash<tensorflow::ModelSignature> {
  size_t operator()(const tensorflow::ModelSignature& signature) const noexcept {
    return signature.hash();
  }
};
}  // namespace std

#endif
