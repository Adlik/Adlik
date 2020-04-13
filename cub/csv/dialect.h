// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_CUB_CSV_DIALECT_H
#define ADLIK_SERVING_CUB_CSV_DIALECT_H

#include <string>
#include <vector>

namespace cub {

struct Dialect {
  std::string delimiter_;
  bool skip_initial_space_;
  char line_terminator_;
  char quote_character_;
  bool double_quote_;
  std::vector<char> trim_characters_;
  bool header_;
  bool skip_empty_rows_;

  std::vector<std::string> column_names_;

  Dialect()
      : delimiter_(","),
        skip_initial_space_(false),
        line_terminator_('\n'),
        quote_character_('"'),
        double_quote_(true),
        trim_characters_({}),
        header_(true),
        skip_empty_rows_(true) {
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

  Dialect& trim_characters() {
    return *this;
  }

  template <typename T, typename... Targs>
  Dialect& trim_characters(T character, Targs... Fargs) {
    trim_characters_.push_back(character);
    trim_characters(Fargs...);
    return *this;
  }

  Dialect& column_names() {
    return *this;
  }

  template <typename Test, template <typename...> class Ref>
  struct is_specialization : std::false_type {};

  template <template <typename...> class Ref, typename... Args>
  struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

  template <typename T, typename... Targs>
  typename std::enable_if<!is_specialization<T, std::vector>::value, Dialect&>::type column_names(T column,
                                                                                                  Targs... Fargs) {
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

}  // namespace cub

#endif
