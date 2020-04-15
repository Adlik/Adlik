// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/csv/csv_writer.h"

namespace cub {

CSVWriter::CSVWriter(const std::string& file_name) : file_name(file_name), written_header(false) {
  stream.open(file_name);
}

Dialect& CSVWriter::configureDialect() {
  return dialect;
}

void CSVWriter::writeRow(const Row& row) {
  if (!written_header) {
    writeHeader();
    written_header = true;
  }

  writeInternal(row);
}

void CSVWriter::writeHeader() {
  if (dialect.column_names_.size() == 0)
    return;

  writeInternal(dialect.column_names_);
}

void CSVWriter::writeInternal(const Row& row) {
  auto should_enclose = [&](const std::string& str) {
    return str.size() > 0 && (str.find(dialect.delimiter_) != std::string::npos ||
                              str.find(dialect.line_terminator_) != std::string::npos ||
                              str.find(dialect.quote_character_) != std::string::npos);
  };

  auto enclose = [&](const std::string& str) {
    std::string output = str;
    std::string::size_type position = output.find(dialect.quote_character_, 0);
    while (position != std::string::npos) {
      output.insert(position, 1, dialect.quote_character_);
      position = output.find(dialect.quote_character_, position + 2);
    }
    return dialect.quote_character_ + output + dialect.quote_character_;
  };

  std::string row_str;
  for (size_t i = 0; i < row.size(); ++i) {
    row_str += (should_enclose(row[i]) ? enclose(row[i]) : row[i]);
    if (i != row.size() - 1) {
      row_str += dialect.delimiter_;
    }
  }

  row_str += dialect.line_terminator_;
  stream << row_str;
}

}  // namespace cub
