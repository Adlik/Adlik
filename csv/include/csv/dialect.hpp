/*
 _______  _______  __   __
|      _||       ||  | |  |  Fast CSV Parser for Modern C++
|     |  |  _____||  |_|  |  http://github.com/p-ranav/csv
|     |  | |_____ |       |
|     |  |_____  ||       |
|     |_  _____| | |     |
|_______||_______|  |___|

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2019 Pranav Srinivas Kumar <pranav.srinivas.kumar@gmail.com>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
#include <csv/robin_hood.hpp>
// #include <string_view>
#include <string>
#include <vector>

#include "cub/string/string_view.h"

namespace csv {

struct Dialect {
  std::string delimiter_;
  bool skip_initial_space_;
  bool skip_empty_rows_;
  char line_terminator_;
  char quote_character_;
  bool double_quote_;
  // unordered_flat_map<std::string_view, bool> ignore_columns_;
  unordered_flat_map<cub::StringView, bool> ignore_columns_;
  std::vector<char> trim_characters_;
  std::vector<std::string> column_names_;
  bool header_;

  Dialect()
      : delimiter_(","),
        skip_initial_space_(false),
        line_terminator_('\n'),
        quote_character_('"'),
        double_quote_(true),
        trim_characters_({}),
        header_(true),
        skip_empty_rows_(false) {
  }

  Dialect& delimiter(const std::string& delimiter) {
    delimiter_ = delimiter;
    return *this;
  }

  Dialect& skip_initial_space(bool skip_initial_space) {
    skip_initial_space_ = skip_initial_space;
    return *this;
  }

  Dialect& skip_empty_rows(bool skip_empty_rows) {
    skip_empty_rows_ = skip_empty_rows;
    return *this;
  }

  Dialect& quote_character(char quote_character) {
    quote_character_ = quote_character;
    return *this;
  }

  Dialect& double_quote(bool double_quote) {
    double_quote_ = double_quote;
    return *this;
  }

  // Base case for trim_characters parameter packing
  Dialect& trim_characters() {
    return *this;
  }

  // Parameter packed trim_characters method
  // Accepts a variadic number of characters
  template <typename T, typename... Targs>
  Dialect& trim_characters(T character, Targs... Fargs) {
    trim_characters_.push_back(character);
    trim_characters(Fargs...);
    return *this;
  }

  // Base case for ignore_columns parameter packing
  Dialect& ignore_columns() {
    return *this;
  }

  // Parameter packed trim_characters method
  // Accepts a variadic number of columns
  template <typename T, typename... Targs>
  Dialect& ignore_columns(T column, Targs... Fargs) {
    ignore_columns_[column] = true;
    ignore_columns(Fargs...);
    return *this;
  }

  // Base case for ignore_columns parameter packing
  Dialect& column_names() {
    return *this;
  }

  // Parameter packed trim_characters method
  // Accepts a variadic number of columns
  template <typename T, typename... Targs>
  Dialect& column_names(T column, Targs... Fargs) {
    column_names_.push_back(column);
    column_names(Fargs...);
    return *this;
  }

  Dialect& column_names(const std::vector<std::string>& columns) {
    for (auto& column : columns)
      column_names_.push_back(column);
    return *this;
  }

  Dialect& header(bool header) {
    header_ = header;
    return *this;
  }
};

}  // namespace csv
