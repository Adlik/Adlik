// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HAF2D2ED4_E89B_4845_A2B4_5C89CDB3A57D
#define HAF2D2ED4_E89B_4845_A2B4_5C89CDB3A57D

#include "cub/dci/role.h"

namespace tensorflow {

struct InferentialBatch;
struct ModelSignature;

DEFINE_ROLE(BatchingProcessor) {
  ABSTRACT(void process(const ModelSignature&, const InferentialBatch&));
};

}  // namespace tensorflow

#endif
