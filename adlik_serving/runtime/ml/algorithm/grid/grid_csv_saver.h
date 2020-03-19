// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_CSV_SAVER_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_GRID_CSV_SAVER_H

#include <string>
#include <vector>

#include "csv/writer.hpp"

namespace ml_runtime {

struct GridOutput;

struct GridCsvSaver {
  GridCsvSaver(const std::string& path);
  ~GridCsvSaver();

  bool save(const std::vector<GridOutput>&);
  bool save(const GridOutput&);

private:
  void saveHeader();

  csv::Writer writer;
};

}  // namespace ml_runtime

#endif
