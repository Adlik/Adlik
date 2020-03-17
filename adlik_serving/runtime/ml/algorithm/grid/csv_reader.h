// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_CSV_READER_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_CSV_READER_H

#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <unordered_map>

namespace ml_runtime {

struct CSVReader {
  using Row = std::unordered_map<std::string, std::string>;
  using RowProcessor = std::function<bool(const Row&)>;

  CSVReader(const std::string&, const std::string& delm = ",");

  bool read();

  bool getData(RowProcessor);

  std::vector<std::string> col_names();

private:
  const std::string file_name;
  const std::string delimeter;
  std::ifstream stream;
  std::vector<std::string> cols;
};

}  // namespace ml_runtime

#endif
