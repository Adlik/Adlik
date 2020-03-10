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
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <csv/concurrent_queue.hpp>
#include <csv/dialect.hpp>
#include <csv/robin_hood.hpp>
#include <fstream>
#include <future>
#include <iostream>
#include <iterator>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
// #include <string_view>
#include "cub/string/string_view.h"

namespace csv {

class Reader {
public:
  Reader()
      : filename_(""),
        columns_(0),
        current_dialect_name_("excel"),
        reading_thread_started_(false),
        processing_thread_started_(false),
        row_iterator_index_(0),
        expected_number_of_rows_(0),
        values_ptoken_(ProducerToken(values_)),
        values_ctoken_(ConsumerToken(values_)),
        rows_ptoken_(ProducerToken(rows_)),
        rows_ctoken_(ConsumerToken(rows_)),
        done_index_(0),
        ready_index_(0),
        next_index_(0),
        ignore_columns_enabled_(false),
        trimming_enabled_(false) {
    Dialect unix_dialect;
    unix_dialect.delimiter(",").quote_character('"').double_quote(true).header(true);
    dialects_["unix"] = unix_dialect;

    Dialect excel_dialect;
    excel_dialect.delimiter(",").quote_character('"').double_quote(true).header(true);
    dialects_["excel"] = excel_dialect;

    Dialect excel_tab_dialect;
    excel_tab_dialect.delimiter("\t").quote_character('"').double_quote(true).header(true);
    dialects_["excel_tab"] = excel_tab_dialect;
  }

  ~Reader() {
    if (reading_thread_started_)
      reading_thread_.join();
    if (processing_thread_started_)
      processing_thread_.join();
  }

  bool busy() {
    if (processing_thread_started_) {
      row_iterator_queue_.try_dequeue(done_index_);
      row_iterator_queue_.enqueue(done_index_);
      bool result = (expected_number_of_rows_ == 0 || done_index_ + 1 == expected_number_of_rows_);
      return !result;
    } else
      return true;
  }

  bool done() {
    if (processing_thread_started_) {
      row_iterator_queue_.try_dequeue(done_index_);
      row_iterator_queue_.enqueue(done_index_);
      bool result = (expected_number_of_rows_ == 0 || done_index_ + 1 == expected_number_of_rows_);
      return result;
    } else
      return false;
  }

  bool ready() {
    size_t rows = 0;
    number_of_rows_processed_.try_dequeue(rows);
    row_iterator_queue_.try_dequeue(ready_index_);
    bool result = (ready_index_ < expected_number_of_rows_ && ready_index_ < rows);
    return result;
  }

  unordered_flat_map<cub::StringView, std::string> next_row() {
    row_iterator_queue_.enqueue(next_index_);
    next_index_ += 1;
    unordered_flat_map<cub::StringView, std::string> result;
    rows_.try_dequeue(rows_ctoken_, result);
    return result;
  }

  void read(const std::string& filename, size_t rows) {
    current_dialect_ = dialects_[current_dialect_name_];
    filename_ = filename;
    stream_ = std::ifstream(filename_);
    if (!stream_.is_open()) {
      throw std::runtime_error("error: Failed to open " + filename_);
    }

    expected_number_of_rows_ = rows;

    if (current_dialect_.trim_characters_.size() > 0)
      trimming_enabled_ = true;

    if (current_dialect_.ignore_columns_.size() > 0)
      ignore_columns_enabled_ = true;

    reading_thread_started_ = true;
    reading_thread_ = std::thread(&Reader::read_internal, this);
  }

  void read(const std::string& filename) {
    current_dialect_ = dialects_[current_dialect_name_];
    filename_ = filename;
    stream_ = std::ifstream(filename_);
    if (!stream_.is_open()) {
      throw std::runtime_error("error: Failed to open " + filename_);
    }

    // new lines will be skipped unless we stop it from happening:
    stream_.unsetf(std::ios_base::skipws);
    std::string line;
    while (std::getline(stream_, line)) {
      if (line.size() > 0 && line[line.size() - 1] == '\r')
        line.pop_back();
      if (line != "" || (!current_dialect_.skip_empty_rows_ && line == ""))
        ++expected_number_of_rows_;
    }

    if (current_dialect_.header_ && expected_number_of_rows_ > 0)
      expected_number_of_rows_ -= 1;

    stream_.clear();
    stream_.seekg(0, std::ios::beg);

    if (current_dialect_.trim_characters_.size() > 0)
      trimming_enabled_ = true;

    if (current_dialect_.ignore_columns_.size() > 0)
      ignore_columns_enabled_ = true;

    reading_thread_started_ = true;
    reading_thread_ = std::thread(&Reader::read_internal, this);
  }

  Dialect& configure_dialect(const std::string& dialect_name = "excel") {
    if (dialects_.find(dialect_name) != dialects_.end()) {
      return dialects_[dialect_name];
    } else {
      dialects_[dialect_name] = Dialect();
      current_dialect_name_ = dialect_name;
      return dialects_[dialect_name];
    }
  }

  std::vector<std::string> list_dialects() {
    std::vector<std::string> result;
    for (auto& kvpair : dialects_)
      result.push_back(kvpair.first);
    return result;
  }

  Dialect& get_dialect(const std::string& dialect_name) {
    return dialects_[dialect_name];
  }

  void use_dialect(const std::string& dialect_name) {
    current_dialect_name_ = dialect_name;
    if (dialects_.find(dialect_name) == dialects_.end()) {
      throw std::runtime_error("error: Dialect " + dialect_name + " not found");
    }
  }

  std::vector<unordered_flat_map<cub::StringView, std::string>> rows() {
    std::vector<unordered_flat_map<cub::StringView, std::string>> rows;
    while (!done()) {
      if (ready()) {
        rows.push_back(next_row());
      }
    }
    return std::move(rows);
  }

  std::vector<std::string> cols() {
    return headers_;
  }

  std::pair<size_t, size_t> shape() {
    return {expected_number_of_rows_, columns_};
  }

private:
  bool front(std::string& value) {
    return values_.try_dequeue(values_ctoken_, value);
  }

  void read_internal() {
    // Get current position
    std::streamoff length = stream_.tellg();

    // Get first line and find headers by splitting on delimiters
    std::string first_line;
    getline(stream_, first_line);

    // Under Linux, getline removes \n from the input stream.
    // However, it does not remove the \r
    // Let's remove it
    if (first_line.size() > 0 && first_line[first_line.size() - 1] == '\r') {
      first_line.pop_back();
    }

    split(first_line);
    if (current_dialect_.header_) {
      headers_ = current_split_result_;
    } else {
      headers_.clear();
      if (current_dialect_.column_names_.size() > 0) {
        headers_ = current_dialect_.column_names_;
      } else {
        for (size_t i = 0; i < current_split_result_.size(); i++)
          headers_.push_back(std::to_string(i));
      }
      // return to start before getline()
      stream_.seekg(length, std::ios_base::beg);
    }

    columns_ = headers_.size();

    for (auto& header : headers_)
      current_row_[header] = "";
    if (ignore_columns_enabled_)
      for (auto& kvpair : current_dialect_.ignore_columns_)
        current_row_.erase(kvpair.first);

    // Start processing thread
    processing_thread_ = std::thread(&Reader::process_values, this);
    processing_thread_started_ = true;

    // Get lines one at a time, split on the delimiter and
    // enqueue the split results into the values_ queue
    bool skip_empty_rows = current_dialect_.skip_empty_rows_;
    std::string row;
    size_t number_of_rows = 0;
    while (std::getline(stream_, row)) {
      if (number_of_rows == expected_number_of_rows_)
        break;
      if (row.size() > 0 && row[row.size() - 1] == '\r')
        row.pop_back();
      if (row != "" || (!skip_empty_rows && row == "")) {
        split(row);
        for (auto& value : current_split_result_)
          values_.enqueue(values_ptoken_, value);
        number_of_rows += 1;
      }
    }
    stream_.close();
  }

  void process_values() {
    size_t index = 0;
    auto ignore_columns = current_dialect_.ignore_columns_;
    size_t i;
    cub::StringView column_name;
    size_t number_of_rows = 0;
    while (number_of_rows < expected_number_of_rows_) {
      if (front(current_value_)) {
        i = index % columns_;
        column_name = headers_[i];
        if (!ignore_columns_enabled_ || ignore_columns.count(column_name) == 0)
          current_row_[column_name] = current_value_;
        index += 1;
        if (index != 0 && index % columns_ == 0) {
          rows_.try_enqueue(current_row_);
          number_of_rows += 1;
          number_of_rows_processed_.enqueue(number_of_rows);
        }
      }
    }
  }

  // trim white spaces from the left end of an input string
  std::string ltrim(std::string const& input) {
    std::string result = input;
    result.erase(
        result.begin(), std::find_if(result.begin(), result.end(), [=](int ch) {
          return !(std::find(current_dialect_.trim_characters_.begin(), current_dialect_.trim_characters_.end(), ch) !=
                   current_dialect_.trim_characters_.end());
        }));
    return std::move(result);
  }

  // trim white spaces from right end of an input string
  std::string rtrim(std::string const& input) {
    std::string result = input;
    result.erase(std::find_if(result.rbegin(),
                              result.rend(),
                              [=](int ch) {
                                return !(std::find(current_dialect_.trim_characters_.begin(),
                                                   current_dialect_.trim_characters_.end(),
                                                   ch) != current_dialect_.trim_characters_.end());
                              })
                     .base(),
                 result.end());
    return std::move(result);
  }

  // trim white spaces from either end of an input string
  std::string trim(std::string const& input) {
    if (current_dialect_.trim_characters_.size() == 0)
      return input;
    return ltrim(rtrim(input));
  }

  // split string based on a delimiter sub-string
  void split(std::string const& input_string) {
    current_split_result_.clear();
    if (input_string == "") {
      current_split_result_ = std::vector<std::string>(columns_, "");
    }

    std::string sub_result = "";
    bool discard_delimiter = false;
    size_t quotes_encountered = 0;
    size_t input_string_size = input_string.size();

    for (size_t i = 0; i < input_string_size; ++i) {
      // Check if ch is the start of a delimiter sequence
      bool delimiter_detected = false;
      for (size_t j = 0; j < current_dialect_.delimiter_.size(); ++j) {
        char ch = input_string[i];
        if (ch != current_dialect_.delimiter_[j]) {
          delimiter_detected = false;
          break;
        } else {
          // ch *might* be the start of a delimiter sequence
          if (j + 1 == current_dialect_.delimiter_.size()) {
            if (quotes_encountered % 2 == 0) {
              // Reached end of delimiter sequence without breaking
              // delimiter detected!
              delimiter_detected = true;
              current_split_result_.push_back(trimming_enabled_ ? trim(sub_result) : sub_result);
              sub_result = "";

              // If enabled, skip initial space right after delimiter
              if (i + 1 < input_string_size) {
                if (current_dialect_.skip_initial_space_ && input_string[i + 1] == ' ') {
                  i = i + 1;
                }
              }
              quotes_encountered = 0;
            } else {
              sub_result += input_string[i];
              i = i + 1;
              if (i == input_string_size)
                break;
            }
          } else {
            // Keep looking
            i = i + 1;
            if (i == input_string_size)
              break;
          }
        }
      }

      // base case
      if (!delimiter_detected)
        sub_result += input_string[i];

      if (input_string[i] == current_dialect_.quote_character_)
        quotes_encountered += 1;
      if (input_string[i] == current_dialect_.quote_character_ && current_dialect_.double_quote_ &&
          sub_result.size() >= 2 && sub_result[sub_result.size() - 2] == input_string[i])
        quotes_encountered -= 1;
    }

    if (sub_result != "")
      current_split_result_.push_back(trimming_enabled_ ? trim(sub_result) : sub_result);

    if (current_split_result_.size() < columns_) {
      for (size_t i = current_split_result_.size(); i < columns_; i++) {
        current_split_result_.push_back("");
      }
    } else if (current_split_result_.size() > columns_ && columns_ != 0) {
      current_split_result_.resize(columns_);
    }
  }

  std::string filename_;
  std::ifstream stream_;
  std::vector<std::string> headers_;
  unordered_flat_map<cub::StringView, std::string> current_row_;
  std::string current_value_;
  ConcurrentQueue<unordered_flat_map<cub::StringView, std::string>> rows_;
  ProducerToken rows_ptoken_;
  ConsumerToken rows_ctoken_;
  ConcurrentQueue<size_t> number_of_rows_processed_;

  // Member variables to keep track of rows/cols
  size_t columns_;
  size_t expected_number_of_rows_;

  // Member variables to enable streaming
  ConcurrentQueue<size_t> row_iterator_queue_;
  size_t row_iterator_index_;

  std::thread reading_thread_;
  bool reading_thread_started_;

  std::thread processing_thread_;
  std::atomic<bool> processing_thread_started_;

  ConcurrentQueue<std::string> values_;
  ProducerToken values_ptoken_;
  ConsumerToken values_ctoken_;
  std::string current_dialect_name_;
  unordered_flat_map<std::string, Dialect> dialects_;
  Dialect current_dialect_;
  size_t done_index_;
  size_t ready_index_;
  size_t next_index_;
  bool ignore_columns_enabled_;
  bool trimming_enabled_;
  std::vector<std::string> current_split_result_;
};

}  // namespace csv
