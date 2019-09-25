#ifndef FRAMEWORK_DOMAIN_DIMS_LIST_H_
#define FRAMEWORK_DOMAIN_DIMS_LIST_H_

#include "google/protobuf/repeated_field.h"
#include "google/protobuf/stubs/port.h"

namespace adlik {
namespace serving {

using DimsList = ::google::protobuf::RepeatedField<::google::protobuf::int64>;
}
}  // namespace adlik

#endif
