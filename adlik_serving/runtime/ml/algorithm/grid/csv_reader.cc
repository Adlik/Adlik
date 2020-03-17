// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/grid/csv_reader.h"

#include "cub/log/log.h"
#include "cub/string/str_utils.h"

namespace ml_runtime {

CSVReader::CSVReader(const std::string& filename, const std::string& delm)
    : file_name(filename), delimeter(delm), stream(file_name) {
}

bool CSVReader::read() {
  if (!stream.is_open()) {
    ERR_LOG << "Input file '" << file_name << "' can't open!";
    return false;
  }

  std::string line = "";
  if (std::getline(stream, line)) {
    cols = cub::strutils::split(line, delimeter);
    for (auto& s : cols) {
      s.erase(s.find_last_not_of(" \n\r\t") + 1);
    }
  }

  return cols.size() != 0;
}

bool CSVReader::getData(RowProcessor func) {
  if (cols.size() == 0) {
    ERR_LOG << "Columns' names is empty, should call read() first!";
    return false;
  }

  std::string line = "";
  while (std::getline(stream, line)) {
    auto elements = cub::strutils::split(line, delimeter);
    if (elements.size() != cols.size()) {
      ERR_LOG << "Read elements' size not equal to headers' size, elements: " << elements.size()
              << ", headers: " << cols.size();
      return false;
    }
    Row row;
    for (auto key = cols.begin(), value = elements.begin(); key != cols.end() && value != elements.end();
         ++key, ++value) {
      row.insert({*key, *value});
    }
    if (!func(row)) {
      ERR_LOG << "Call RowProcess function failure!";
      return false;
    }
  }
  return true;
}

std::vector<std::string> CSVReader::col_names() {
  return cols;
}

}  // namespace ml_runtime
