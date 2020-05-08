// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_CSV_SAVER_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_CSV_SAVER_H

#include <string>
#include <vector>

#include "cub/csv/csv_writer.h"

namespace ml_runtime {

struct GridOutput;

struct GridCsvSaver {
  GridCsvSaver(const std::string& path);

  bool save(const std::vector<GridOutput>&);
  bool save(const GridOutput&);

private:
  cub::CSVWriter writer;
};

}  // namespace ml_runtime

#endif
