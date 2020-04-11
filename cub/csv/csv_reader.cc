// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/csv/csv_reader.h"

#include <algorithm>

#include "cub/log/log.h"
#include "cub/string/str_utils.h"

namespace cub {

namespace {

struct Helper {
  Helper(const Dialect& dialect) : dialect(dialect) {
  }

  std::string trim(std::string const& input) const {
    return trimEnabled() ? ltrim(rtrim(input)) : input;
  }

  bool trimEnabled() const {
    return dialect.trim_characters_.size() > 0;
  }

  std::string ltrim(std::string const& input) const {
    std::string result = input;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [=](int ch) {
                   return !(std::find(dialect.trim_characters_.begin(), dialect.trim_characters_.end(), ch) !=
                            dialect.trim_characters_.end());
                 }));
    return result;
  }

  std::string rtrim(std::string const& input) const {
    std::string result = input;
    result.erase(
        std::find_if(result.rbegin(),
                     result.rend(),
                     [=](int ch) {
                       return !(std::find(dialect.trim_characters_.begin(), dialect.trim_characters_.end(), ch) !=
                                dialect.trim_characters_.end());
                     })
            .base(),
        result.end());
    return result;
  }

  const Dialect& dialect;
};
}  // namespace

CSVReader::CSVReader(const std::string& filename)
    : file_name(filename), stream(file_name), columns(0), read_header(false) {
  checkOpen();
}

std::vector<std::string> CSVReader::cols() {
  if (!read_header) {
    readHeader();
    read_header = true;
  }
  return header;
}

Dialect& CSVReader::configureDialect() {
  return dialect;
}

std::vector<CSVReader::Row> CSVReader::rows() {
  std::vector<CSVReader::Row> rows;
  Row row;
  while (nextRow(row)) {
    rows.push_back(std::move(row));
  }
  return std::move(rows);
}

bool CSVReader::nextRow(Row& row) {
  row.clear();

  if (!read_header) {
    readHeader();
    read_header = true;
  }

  std::string line = "";
  if (getLine(line)) {
    auto elements = splitLine(line);
    for (auto key = header.begin(), value = elements.begin(); key != header.end() && value != elements.end();
         ++key, ++value) {
      row.insert({*key, *value});
    }
    return true;
  }
  return false;
}

void CSVReader::checkOpen() {
  if (!stream.is_open()) {
    std::ostringstream oss;
    oss << "Cannot open file '" << file_name << "'. " << std::endl;
    throw std::runtime_error(oss.str());
  }
  stream.unsetf(std::ios::skipws);  // don't ignore whitespace
}

void CSVReader::readHeader() {
  checkOpen();

  if (dialect.header_) {
    std::string line = "";
    if (getLine(line)) {
      header = splitLine(line);
    }
  } else if (dialect.column_names_.size() > 0) {
    header = dialect.column_names_;
  } else {
    std::string line = "";
    getLine(line);
    stream.seekg(0);  // skip to the beginning of file
    auto elements = splitLine(line);  // just use the number of fields
    for (size_t i = 0; i < elements.size(); ++i) {
      header.push_back(std::to_string(i));
    }
  }
  columns = header.size();
}

bool CSVReader::getLine(std::string& line) {
  if (std::getline(stream, line)) {
    if (line.size() > 0 && line[line.size() - 1] == '\r')
      line.pop_back();
    if (line.size() == 0 && dialect.skip_empty_rows_) {
      return getLine(line);
    }
    return true;
  } else {
    return false;
  }
}

std::vector<std::string> CSVReader::splitLine(const std::string& line) const {
  std::vector<std::string> result;

  if (line.length() == 0) {
    result.assign(columns, "");
    return std::move(result);
  }

  size_t quotes_encountered = 0;
  std::string sub_result = "";

  for (size_t i = 0; i < line.length(); ++i) {
    bool delimiter_detected = false;
    for (size_t j = 0; j < dialect.delimiter_.size(); ++j) {
      char ch = line[i];
      if (ch != dialect.delimiter_[j]) {
        delimiter_detected = false;
        break;
      } else {
        // ch *might* be the start of a delimiter sequence
        if (j + 1 == dialect.delimiter_.size()) {
          if (quotes_encountered % 2 == 0) {
            delimiter_detected = true;
            result.push_back(Helper(dialect).trim(sub_result));  // maybe should trim the field
            sub_result = "";

            // If enabled, skip initial space right after delimiter
            if (i + 1 < line.length()) {
              if (dialect.skip_initial_space_ && line[i + 1] == ' ') {
                i = i + 1;
              }
            }
            quotes_encountered = 0;
          } else {
            sub_result += line[i];
            i = i + 1;
            if (i == line.length())
              break;
          }
        } else {
          i = i + 1;
          if (i == line.length())
            break;
        }
      }
    }

    if (!delimiter_detected)
      sub_result += line[i];

    if (line[i] == dialect.quote_character_)
      quotes_encountered += 1;
    if (line[i] == dialect.quote_character_ && dialect.double_quote_ && sub_result.size() >= 2 &&
        sub_result[sub_result.size() - 2] == line[i])
      quotes_encountered -= 1;
  }

  if (sub_result != "") {
    result.push_back(Helper(dialect).trim(sub_result));
  }

  if (result.size() < columns) {
    for (size_t i = result.size(); i < columns; i++) {
      result.push_back("");
    }
  } else if (result.size() > columns && columns != 0) {
    result.resize(columns);
  }

  return std::move(result);
}

}  // namespace cub
