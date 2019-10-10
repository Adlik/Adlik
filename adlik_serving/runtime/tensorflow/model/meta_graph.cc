// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/model/meta_graph.h"

namespace tensorflow {

MetaGraph::MetaGraph(MetaGraphDef& graph) {
  for (auto& entry : graph.signature_def()) {
    list.push_back(entry.second);
  }
}

}  // namespace tensorflow
