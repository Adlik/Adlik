// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HE7018187_E230_4AB2_A4D0_9B93A1B1FA77
#define HE7018187_E230_4AB2_A4D0_9B93A1B1FA77

#include <vector>

#include "adlik_serving/runtime/tensorflow/model/model_signature.h"
#include "tensorflow/core/protobuf/meta_graph.pb.h"

namespace tensorflow {

struct MetaGraphDef;

struct MetaGraph {
  MetaGraph(MetaGraphDef&);

  template <typename F>
  void signatures(F f) const {
    for (auto& signature : list) {
      f(signature);
    }
  }

private:
  std::vector<ModelSignature> list;
};

}  // namespace tensorflow

#endif
