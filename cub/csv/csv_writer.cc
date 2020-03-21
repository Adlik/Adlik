// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/csv/csv_writer.h"

#include "cub/string/str_utils.h"

namespace cub {

CSVWriter::CSVWriter(const std::string& file_name, const std::string& delm) : file_name(file_name), delimeter(delm) {
  stream.open(file_name);
}

void CSVWriter::writeRow(const Row& row) {
  auto row_str = cub::strutils::join(row, delimeter);
  stream << row_str << '\n';
}

void CSVWriter::close() {
  stream.close();
}

}  // namespace cub
