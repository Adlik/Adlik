// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef CUB_CSV_CSV_WRITER_H
#define CUB_CSV_CSV_WRITER_H

#include <fstream>
#include <string>
#include <vector>

namespace cub {

struct CSVWriter {
  using Row = std::vector<std::string>;

  CSVWriter(const std::string&, const std::string& delm = ",");

  void writeRow(const Row&);

private:
  const std::string file_name;
  const std::string delimeter;
  std::ofstream stream;
};

}  // namespace cub

#endif
