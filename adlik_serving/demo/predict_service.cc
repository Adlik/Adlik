// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/demo/predict_service.h"
#include <array>
#include "adlik_serving/framework/manager/runtime_suite.h"

using adlik::serving::RuntimeSuite;
using adlik::serving::demo::PredictService;
using cub::ProgramOptions;
using std::string;

PredictService::PredictService(const char* model_repository) {
  ProgramOptions program_options;

  ModelOptions::subscribe(program_options);

  string model_base_path_arg{"--model_base_path="};

  model_base_path_arg += model_repository;

  const char* arguments[] = {"unused", model_base_path_arg.c_str(), nullptr};
  const auto argc = static_cast<int>(std::end(arguments) - std::begin(arguments)) - 1;

  if (!program_options.parse(argc, arguments)) {
    throw std::logic_error("Invalid arguments.");
  }
}

void PredictService::start() {
  // Configure.

  if (cub::isFailStatus(static_cast<Runtime&>(RuntimeSuite::inst()).config(*this))) {
    throw std::runtime_error("Failed to configure runtime suite.");
  }

  if (cub::isFailStatus(ModelStore::config())) {
    throw std::runtime_error("Failed to configure model store.");
  }

  StateMonitor::connect(*this);
  static_cast<ModelSource&>(*this).connect(*this);

  StorageLoop::poll();
  BoardingLoop::poll();

  StateMonitor::wait();
}
