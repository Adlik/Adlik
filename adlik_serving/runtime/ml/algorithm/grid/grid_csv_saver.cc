// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/grid/grid_csv_saver.h"

#include "adlik_serving/runtime/ml/algorithm/grid/grid_output.h"

namespace ml_runtime {

GridCsvSaver::GridCsvSaver(const std::string& path) : writer(path) {
  writer.configureDialect().column_names(GridOutput::fieldNames());
}

bool GridCsvSaver::save(const std::vector<GridOutput>& outputs) {
  for (const auto& o : outputs) {
    save(o);
  }
  return true;
}

bool GridCsvSaver::save(const GridOutput& output) {
  std::vector<std::string> row = output.toString();
  writer.writeRow(row);
  return true;
}

}  // namespace ml_runtime
