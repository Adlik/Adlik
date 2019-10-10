// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/framework/domain/model_config_helper.h"

#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/platform/env.h"

#if GOOGLE_CUDA

#include <cuda_runtime_api.h>

#endif

namespace adlik {
namespace serving {

size_t GetDataTypeByteSize(const tensorflow::DataType dtype) {
  switch (dtype) {
    case tensorflow::DataType::DT_BOOL:
      return 1;
    case tensorflow::DataType::DT_UINT8:
      return 1;
    case tensorflow::DataType::DT_UINT16:
      return 2;
    case tensorflow::DataType::DT_UINT32:
      return 4;
    case tensorflow::DataType::DT_UINT64:
      return 8;
    case tensorflow::DataType::DT_INT8:
      return 1;
    case tensorflow::DataType::DT_INT16:
      return 2;
    case tensorflow::DataType::DT_INT32:
      return 4;
    case tensorflow::DataType::DT_INT64:
      return 8;
    case tensorflow::DataType::DT_HALF:
      return 2;
    case tensorflow::DataType::DT_FLOAT:
      return 4;
    case tensorflow::DataType::DT_DOUBLE:
      return 8;
    default:
      break;
  }

  return 0;
}

uint64_t GetSize(const tensorflow::DataType& dtype, const DimsList& dims) {
  size_t dt_size = GetDataTypeByteSize(dtype);
  if (dt_size <= 0) {
    return 0;
  }

  int64_t size = 0;
  for (auto dim : dims) {
    if (size == 0) {
      size = dim;
    } else {
      size *= dim;
    }
  }

  return size * dt_size;
}

uint64_t GetSize(const ModelInput& mio) {
  return GetSize(mio.data_type(), mio.dims());
}

uint64_t GetSize(const ModelOutput& mio) {
  return GetSize(mio.data_type(), mio.dims());
}

uint64_t GetSize(const int max_batch_size, const tensorflow::DataType& dtype, const DimsList& dims) {
  size_t dt_size = GetSize(dtype, dims);
  return std::max(1, max_batch_size) * dt_size;
}

tensorflow::Status NormalizeModelConfig(ModelConfigProto& config) {
  // If version_policy is not specified, default to Latest 1 version.
  if (!config.has_version_policy()) {
    VersionPolicyProto::Latest latest;
    latest.set_num_versions(1);
    config.mutable_version_policy()->mutable_latest()->CopyFrom(latest);
  }
  // If dynamic batching is specified...
  if (config.has_dynamic_batching()) {
    // If preferred batch size is not specified choose
    // automatically. For now we just choose 4, 8 as those are
    // generally good values for GPUs.
    if (config.dynamic_batching().preferred_batch_size().size() == 0) {
      if (config.max_batch_size() >= 4) {
        config.mutable_dynamic_batching()->mutable_preferred_batch_size()->Add(4);
      }
      if (config.max_batch_size() >= 8) {
        config.mutable_dynamic_batching()->mutable_preferred_batch_size()->Add(8);
      }
    }
  }

  // Make sure there is at least one instance_group.
  if (config.instance_group().size() == 0) {
    ModelInstanceGroup* group = config.add_instance_group();
    group->set_name(config.name());
  }

  int device_cnt = 0;
#if GOOGLE_CUDA
  cudaError_t cuerr = cudaGetDeviceCount(&device_cnt);
  if (cuerr == cudaErrorNoDevice) {
    device_cnt = 0;
  } else if (cuerr != cudaSuccess) {
    return tensorflow::errors::Internal(
        "unable to get number of CUDA devices for ", config.name(), ": ", cudaGetErrorString(cuerr));
  }
#endif

  // Assign default name, kind and count to each instance group that
  // doesn't give those values explicitly. For KIND_GPU, set GPUs to
  // all available if not specified explicitly.
  size_t cnt = 0;
  for (auto& group : *config.mutable_instance_group()) {
    // Name
    if (group.name().empty()) {
      group.set_name(config.name() + "_" + std::to_string(cnt));
    }
    cnt++;

    // For KIND_AUTO... if there are no GPUs or if any of the listed
    // 'gpu's are not present, then use KIND_CPU.
    if (group.kind() == ModelInstanceGroup::KIND_AUTO) {
      if (device_cnt == 0) {
        group.set_kind(ModelInstanceGroup::KIND_CPU);
      } else {
        for (const int32_t gid : group.gpus()) {
          if ((gid < 0) || (gid >= device_cnt)) {
            group.set_kind(ModelInstanceGroup::KIND_CPU);
            break;
          }
        }
      }

      if (group.kind() == ModelInstanceGroup::KIND_AUTO) {
        group.set_kind(ModelInstanceGroup::KIND_GPU);
      }
    }

    // Count
    if (group.count() < 1) {
      group.set_count(1);
    }

    // GPUs
    if ((group.kind() == ModelInstanceGroup::KIND_GPU) && (group.gpus().size() == 0)) {
      for (int d = 0; d < device_cnt; d++) {
        group.add_gpus(d);
      }
    }
  }
  return tensorflow::Status::OK();
}

tensorflow::Status ValidateModelConfig(const ModelConfigProto& config) {
  if (config.instance_group().size() == 0) {
    return tensorflow::errors::InvalidArgument("must specify one or more 'instance group's for ", config.name());
  }

  // If dynamic batching is specified make sure the preferred batch
  // sizes are positive and don't exceed maximum batch size. Make sure
  // the max delay is non-negative.
  if (config.has_dynamic_batching()) {
    for (const auto size : config.dynamic_batching().preferred_batch_size()) {
      if (size <= 0) {
        return tensorflow::errors::InvalidArgument("dynamic batching preferred size must be positive for ",
                                                   config.name());
      }
      if (size > config.max_batch_size()) {
        return tensorflow::errors::InvalidArgument("dynamic batching preferred size must be <= max batch size for ",
                                                   config.name());
      }
    }

    if (config.dynamic_batching().max_queue_delay_microseconds() < 0) {
      return tensorflow::errors::InvalidArgument("dynamic batching maximum queue delay must be non-negative for ",
                                                 config.name());
    }
  }

#if GOOGLE_CUDA
  // Make sure KIND_GPU instance group specifies at least one GPU and
  // doesn't specify a non-existent GPU. Make sure non-KIND_GPU does
  // not specify any GPUs.
  int dcnt;
  cudaError_t cuerr = cudaGetDeviceCount(&dcnt);
  if (cuerr == cudaErrorNoDevice) {
    dcnt = 0;
  } else if (cuerr != cudaSuccess) {
    return tensorflow::errors::Internal(
        "failed to get device count for validation of model ", config.name(), ": ", cudaGetErrorString(cuerr));
  }

  for (const auto& group : config.instance_group()) {
    if (group.kind() == ModelInstanceGroup::KIND_GPU) {
      if (group.gpus().size() == 0) {
        return tensorflow::errors::InvalidArgument(
            "instance group ", group.name(), " of model ", config.name(), " has kind KIND_GPU but specifies no GPUs");
      }

      for (const int32_t gid : group.gpus()) {
        if ((gid < 0) || (gid >= dcnt)) {
          return tensorflow::errors::InvalidArgument("instance group ",
                                                     group.name(),
                                                     " of model ",
                                                     config.name(),
                                                     " specifies invalid GPU id ",
                                                     gid,
                                                     ", valid GPUs are 0 - ",
                                                     (dcnt - 1));
        }
      }
    } else if (group.kind() == ModelInstanceGroup::KIND_CPU) {
      if (group.gpus().size() > 0) {
        return tensorflow::errors::InvalidArgument("instance group ",
                                                   group.name(),
                                                   " of model ",
                                                   config.name(),
                                                   " has kind KIND_CPU but specifies one or more GPUs");
      }
    } else {
      return tensorflow::errors::Internal(
          "instance group ", group.name(), " of model ", config.name(), " has unexpected kind KIND_AUTO");
    }
  }
#endif
  return tensorflow::Status::OK();
}

}  // namespace serving
}  // namespace adlik
