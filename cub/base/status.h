#ifndef H5712E60C_DEF5_4A3B_93C0_C074CD80C63BXXX
#define H5712E60C_DEF5_4A3B_93C0_C074CD80C63BXXX

#include "stdint.h"

namespace cub {

using Status = uint32_t;

constexpr Status ReservedFailure = 0x80000000;

constexpr Status succStatus(Status status) {
  return status;
}

constexpr Status failStatus(Status status) {
  return status | ReservedFailure;
}

constexpr bool isSuccStatus(Status status) {
  return (status & ReservedFailure) == 0;
}

constexpr bool isFailStatus(Status status) {
  return !isSuccStatus(status);
}

enum : Status {
  // OK
  Success = succStatus(0),
  Continue = succStatus(1),

  // Error
  Cancelled = failStatus(1),
  InvalidArgument = failStatus(2),
  NotFound = failStatus(3),
  AlreadyExists = failStatus(4),
  ResourceExhausted = failStatus(5),
  Unavailable = failStatus(6),
  FailedPrecondition = failStatus(7),
  OutOfRange = failStatus(8),
  Unimplemented = failStatus(9),
  Internal = failStatus(10),
  Aborted = failStatus(11),
  DeadlineExceeded = failStatus(12),
  DataLoss = failStatus(13),
  Unknown = failStatus(14),
  PermissionDenied = failStatus(15),
  Unauthenticated = failStatus(16),

  // Reserved Error
  FatalBug = failStatus(0x7FFFFFFE),
  Failure = failStatus(0x7FFFFFFF),
};

/////////////////////////////////////////////////////////////////
#define __CUB_FAILED(status) cub::isFailStatus(status)
#define CUB_IS_SUCC_STATUS(status) cub::isSuccStatus(status)

}  // namespace cub

#endif
