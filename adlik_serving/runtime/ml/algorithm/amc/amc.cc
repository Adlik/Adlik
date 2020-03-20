// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <memory>

#include "adlik_serving/apis/amc_task.pb.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm.h"
#include "adlik_serving/runtime/ml/algorithm/algorithm_register.h"
#include "adlik_serving/runtime/ml/algorithm/proto/amc.pb.h"
#include "cub/env/fs/file_system.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"
#include "cub/protobuf/text_protobuf.h"
#include "google/protobuf/any.pb.h"

namespace ml_runtime {

struct Amc : Algorithm {
  static cub::StatusWrapper create(const std::string&, std::unique_ptr<Algorithm>*);

  Amc(const adlik::serving::AmcConfig& config) : config(config) {
    init();
  }

  cub::StatusWrapper run(const ::google::protobuf::Any&, ::google::protobuf::Any&) override;

private:
  struct Parameter {
    double target_prime;
    double lambda;
  };
  using CellId = unsigned int;

  void init() {
    for (const auto& it : config.cell_parameters()) {
      cells.insert({it.first, {(1 - it.second.nbler_target()) / it.second.nbler_target(), it.second.lambda()}});
    }
  }

  adlik::serving::AmcConfig config;

  std::unordered_map<CellId, Parameter> cells;
};

cub::StatusWrapper Amc::create(const std::string& model_dir, std::unique_ptr<Algorithm>* algorithm) {
  auto file_path = cub::paths(model_dir, DEFAULT_MODEL);
  if (!cub::filesystem().exists(file_path)) {
    ERR_LOG << file_path << " not exist!";
    return cub::StatusWrapper(cub::Internal, "model.pbtxt not exist");
  }

  auto config = cub::TextProtobuf::read<::adlik::serving::AmcConfig>(file_path);
  if (config.cell_parameters().size() == 0) {
    ERR_LOG << "nbler_target is 0!";
    return cub::StatusWrapper(cub::InvalidArgument, "nbler_target is 0");
  }
  *algorithm = std::make_unique<Amc>(config);
  return cub::StatusWrapper::OK();
}

cub::StatusWrapper Amc::run(const ::google::protobuf::Any& request, ::google::protobuf::Any& rsp_detail) {
  DEBUG_LOG << "Prepare to run amc, this: " << this;

  if (!request.Is<::adlik::serving::AmcTaskReq>()) {
    return cub::StatusWrapper(cub::InvalidArgument, "Input doesn't contain amc task config!");
  }

  adlik::serving::AmcTaskReq task;
  request.UnpackTo(&task);

  adlik::serving::AmcTaskRsp output;
  output.set_cell_id(task.cell_id());

  auto search = cells.find(task.cell_id());
  if (search != cells.end()) {
    auto deltas = output.mutable_deltas();
    for (const auto& it : task.blers()) {
      auto delta = it.second;
      deltas->insert({it.first, (1 - delta - delta * search->second.target_prime) * search->second.lambda});
    }
    rsp_detail.PackFrom(output);
    return cub::StatusWrapper::OK();
  } else {
    ERR_LOG << "Not found input cell id: " << task.cell_id();
    return cub::StatusWrapper(cub::InvalidArgument, "Not found input cell id:");
  }
}

REGISTER_ALGORITHM(Amc, "amc");

}  // namespace ml_runtime
