// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/server/http/internal/operation_tracker.h"

namespace adlik {
namespace serving {

void OperationTracker::incOps() {
  absl::MutexLock l(&ops_mu);
  num_pending_ops++;
}

void OperationTracker::decOps() {
  absl::MutexLock l(&ops_mu);
  num_pending_ops--;
}

void OperationTracker::waitDone() {
  absl::MutexLock l(&ops_mu);
  ops_mu.Await(absl::Condition(
      +[](int64_t* count) { return *count <= 0; }, &num_pending_ops));
}

void OperationTracker::waitToOne() {
  absl::MutexLock l(&ops_mu);
  ops_mu.Await(absl::Condition(
      +[](int64_t* count) { return *count <= 1; }, &num_pending_ops));
}

}  // namespace serving
}  // namespace adlik
