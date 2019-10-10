// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/tensorflow/batch/final_padding.h"

namespace tensorflow {

FinalPadding& FinalPadding::nil() {
  static FinalPadding inst;
  return inst;
}

}  // namespace tensorflow
