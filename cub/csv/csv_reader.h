// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_CUB_CSV_CSV_READER_H
#define ADLIK_SERVING_CUB_CSV_CSV_READER_H

#include <fstream>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace cub {

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

}  // namespace cub

#endif
