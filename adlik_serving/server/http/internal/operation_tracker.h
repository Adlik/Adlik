// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_SERVER_HTTP_INTERNAL_OPERATION_TRACKER_H
#define ADLIK_SERVING_SERVER_HTTP_INTERNAL_OPERATION_TRACKER_H

#include "absl/synchronization/mutex.h"

namespace adlik {
namespace serving {

struct OperationTracker {
  void incOps();
  void decOps();

  void waitDone();
  void waitToOne();

private:
  mutable absl::Mutex ops_mu;
  int64_t num_pending_ops TF_GUARDED_BY(ops_mu) = 0;
};

}  // namespace serving
}  // namespace adlik

#endif
