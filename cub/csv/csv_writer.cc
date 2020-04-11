// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/csv/csv_writer.h"

#include "cub/string/str_utils.h"

namespace cub {

CSVWriter::CSVWriter(const std::string& file_name) : file_name(file_name), written_header(false) {
  stream.open(file_name);
}

Dialect& CSVWriter::configureDialect() {
  return dialect;
}

void CSVWriter::writeRow(const Row& row) {
  if (!written_header) {
    writerHeader();
    written_header = true;
  }

  std::string row_str;
  for (size_t i = 0; i < row.size(); ++i) {
    row_str += row[i];
    if (i + 1 < row.size())
      row_str += dialect.delimiter_;
  }
  row_str += dialect.line_terminator_;
  stream << row_str;
}

void CSVWriter::writerHeader() {
  auto column_names = dialect.column_names_;
  if (column_names.size() == 0)
    return;
  std::string row;
  for (size_t i = 0; i < column_names.size(); i++) {
    row += column_names[i];
    if (i + 1 < column_names.size())
      row += dialect.delimiter_;
  }
  row += dialect.line_terminator_;
  stream << row;
}

}  // namespace cub
