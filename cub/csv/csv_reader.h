// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

// The csv format reference https://tools.ietf.org/html/rfc4180

#ifndef ADLIK_SERVING_CUB_CSV_CSV_READER_H
#define ADLIK_SERVING_CUB_CSV_CSV_READER_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "cub/csv/dialect.h"
#include "cub/string/string_view.h"
#include "cub/string/string_view_hash.h"

namespace cub {

struct CSVReader {
  using Row = std::unordered_map<StringView, std::string>;

  CSVReader(const std::string&);

  Dialect& configureDialect();

  std::vector<std::string> cols();
  std::vector<Row> rows();
  bool nextRow(Row&);

private:
  void checkOpen();
  void readHeader();
  bool readInternal(std::vector<std::string>&);
  bool getLine(std::string&);

  const std::string file_name;
  std::ifstream stream;
  std::vector<std::string> header;
  size_t columns;
  Dialect dialect;
  bool read_header;
};

}  // namespace cub

#endif
