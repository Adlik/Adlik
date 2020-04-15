// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/csv/csv_reader.h"

#include <algorithm>

#include "cub/log/log.h"
#include "cub/string/str_utils.h"

namespace cub {

namespace {

struct TrimHelper {
  TrimHelper(const std::vector<char>& trim_characters) : trim_characters(trim_characters) {
  }

  std::string trim(std::string const& input) const {
    if (input.size() == 0) {
      return input;
    }
    return trim_characters.size() ? ltrim(rtrim(input)) : input;
  }

private:
  std::string ltrim(const std::string& input) const {
    std::string result = input;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [=](int ch) {
                   return !(std::find(trim_characters.begin(), trim_characters.end(), ch) != trim_characters.end());
                 }));
    return result;
  }

  std::string rtrim(const std::string& input) const {
    std::string result = input;
    result.erase(
        std::find_if(result.rbegin(),
                     result.rend(),
                     [=](int ch) {
                       return !(std::find(trim_characters.begin(), trim_characters.end(), ch) != trim_characters.end());
                     })
            .base(),
        result.end());
    return result;
  }

  const std::vector<char>& trim_characters;
};

struct Field {
  Field(const Dialect& dialect) : dialect(dialect) {
  }

  enum State { INIT, COMMON_WAIT, START_WITH_QUOTE, WAIT_NEXT_QUOTE, WAIT_END_QUOTE, END };

  void append(const char ch, const char next) {
#define DO_ON_STATE(current, func) \
  {                                \
    case current: {                \
      func(ch, next);              \
      break;                       \
    }                              \
  }
    switch (state) {
      DO_ON_STATE(INIT, onInit)
      DO_ON_STATE(COMMON_WAIT, onCommonWait)
      DO_ON_STATE(START_WITH_QUOTE, onStartWithQuote)
      DO_ON_STATE(WAIT_NEXT_QUOTE, onWaitNextQuote)
      DO_ON_STATE(WAIT_END_QUOTE, onWaitEndQuote)
      default:
        return;
    }
  }

  void reset() {
    state = INIT;
    result.clear();
  }

  bool done() const {
    return state == END || state == INIT;
  }

  std::string toString() const {
    return TrimHelper(dialect.trim_characters_).trim(result);
  }

private:
  void onInit(const char ch, const char) {
    if (isEndChar(ch)) {
      state = END;
    } else if (isQuote(ch)) {
      state = START_WITH_QUOTE;
    } else if (ch == ' ' && dialect.skip_initial_space_) {  // do nothing
    } else {
      result += ch;
      state = COMMON_WAIT;
    }
  }

  void onCommonWait(const char ch, const char) {
    if (isEndChar(ch)) {
      state = END;
    } else {
      result += ch;
    }
  }

  void onStartWithQuote(const char ch, const char) {
    result += ch;
    state = isQuote(ch) ? WAIT_NEXT_QUOTE : WAIT_END_QUOTE;
  }

  void onWaitNextQuote(const char ch, const char) {
    if (isQuote(ch)) {
      // two consecutive quotes = one quote, don't need push
      state = WAIT_END_QUOTE;
    } else if (isEndChar(ch)) {
      result.pop_back();
      state = END;
    } else {
      result.pop_back();
      result += ch;
      state = COMMON_WAIT;
    }
  }

  void onWaitEndQuote(const char ch, const char next) {
    if (isQuote(ch)) {
      if (dialect.double_quote_) {
        if (next == 0) {
          state = END;
        } else if (isEndChar(next)) {
          state = COMMON_WAIT;
        } else {
          result += ch;
          state = WAIT_NEXT_QUOTE;
        }
      } else {
        state = COMMON_WAIT;
      }
    } else {
      result += ch;
    }
  }

  bool isEndChar(const char ch) const {
    return ch == dialect.delimiter_ || ch == dialect.line_terminator_;
  }

  bool isQuote(const char ch) const {
    return ch == dialect.quote_character_;
  }

  State state = INIT;
  std::string result;
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

  std::vector<std::string> line;
  if (readInternal(line)) {
    for (auto key = header.begin(), value = line.begin(); key != header.end() && value != line.end(); ++key, ++value) {
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
    readInternal(header);
  } else if (dialect.column_names_.size() > 0) {
    header = dialect.column_names_;
  } else {
    std::vector<std::string> elements;
    readInternal(elements);  // just use the number of fields to name header
    for (size_t i = 0; i < elements.size(); ++i) {
      header.push_back(std::to_string(i));
    }

    stream.seekg(0);  // Note: skip to the beginning of the file because there is no header in the file
  }
  columns = header.size();
}

// return true means read a complete row, return false means no line to read and reach the file end.
// It will throw a runtime error if reach the end but field not completed
bool CSVReader::readInternal(std::vector<std::string>& result) {
  result.clear();

  Field current_field(dialect);

  auto empty = [&](const std::string& input) {
    return input.empty() || (input.size() == 1 && input[0] == dialect.line_terminator_);
  };

  auto func = [&](const std::string& input) {
    for (size_t i = 0; i < input.length(); ++i) {
      current_field.append(input[i], i == input.length() - 1 ? 0 : input[i + 1]);
      if (current_field.done()) {
        result.push_back(current_field.toString());
        current_field.reset();
      }
    }
    return current_field.done();
  };

  bool first_line = true;
  std::string line;
  while (getLine(line)) {
    if (first_line && empty(line) && dialect.skip_empty_rows_) {
      continue;
    }
    if (func(line)) {
      // complete all columns
      if (result.size() < columns) {
        for (size_t i = result.size(); i < columns; i++) {
          result.push_back("");
        }
      } else if (result.size() > columns && columns != 0) {
        result.resize(columns);
      }
      return true;
    }
    first_line = false;
  }

  // reach the file end
  if (current_field.done()) {
    return false;
  }
  // reach the end but field not complete
  std::ostringstream oss;
  oss << "EOF inside string starting at row " << std::endl;
  throw std::runtime_error(oss.str());
}

bool CSVReader::getLine(std::string& line) {
  if (std::getline(stream, line)) {
    if (line.size() > 0 && line[line.size() - 1] == '\r')
      line.pop_back();
    line.push_back(dialect.line_terminator_);
    return true;
  } else {
    return false;
  }
}

}  // namespace cub
