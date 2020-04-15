// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/csv/csv_reader.h"

#include "cub/csv/test_util.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

FIXTURE(CSVReaderTest){TEST("read an empty csv"){CSVReader csv(TEST_FILE("empty.csv"));
ASSERT_EQ(0, csv.rows().size());
ASSERT_EQ(0, csv.cols().size());
}  // namespace cub

TEST("read file that doesn't exist") {
  bool exception_thrown = false;
  try {
    CSVReader csv(TEST_FILE("missing.csv"));
  } catch (std::exception&) {
    exception_thrown = true;
  }
  ASSERT_TRUE(exception_thrown);
}

TEST("read the most basic csv file correctly") {
  CSVReader csv(TEST_FILE("test_01.csv"));

  auto rows = csv.rows();

  std::vector<std::string> expected_header{"a", "b", "c"};
  ASSERT_EQ(expected_header, csv.cols());

  ASSERT_EQ(2, rows.size());
  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "5");
  ASSERT_EQ(rows[1]["c"], "6");
}

TEST("read the most basic csv file correctly with nextRow interface") {
  CSVReader csv(TEST_FILE("test_01.csv"));

  std::vector<CSVReader::Row> rows;
  CSVReader::Row row;
  while (csv.nextRow(row)) {
    rows.push_back(std::move(row));
  }

  ASSERT_EQ(2, rows.size());
  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "5");
  ASSERT_EQ(rows[1]["c"], "6");
}

TEST("read the most basic csv file with ',' delimiter and not trim space") {
  CSVReader csv(TEST_FILE("test_02.csv"));
  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 2);

  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0][" b"], " 2");
  ASSERT_EQ(rows[0][" c"], " 3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1][" b"], " 5");
  ASSERT_EQ(rows[1][" c"], " 6");
}

TEST("read the most basic csv file with ',' delimiter and trim space") {
  CSVReader csv(TEST_FILE("test_02.csv"));
  csv.configureDialect().skip_initial_space(true);

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 2);

  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "5");
  ASSERT_EQ(rows[1]["c"], "6");
}

TEST("read the most basic csv file with ':' delimiter") {
  CSVReader csv(TEST_FILE("test_03.csv"));
  csv.configureDialect().delimiter(':');

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 2);

  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "5");
  ASSERT_EQ(rows[1]["c"], "6");
}

TEST("read the csv file and trim specific characters") {
  CSVReader csv(TEST_FILE("test_02.csv"));
  csv.configureDialect().trim_characters(' ', '\t');

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 2);

  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "5");
  ASSERT_EQ(rows[1]["c"], "6");
}

TEST("read the csv file and trim specific characters which are very special") {
  CSVReader csv(TEST_FILE("test_04.csv"));
  csv.configureDialect().trim_characters(' ', '\t');

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 2);

  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "5");
  ASSERT_EQ(rows[1]["c"], "6");
}

TEST("read the csv file which has no header") {
  CSVReader csv(TEST_FILE("no_header.csv"));
  csv.configureDialect().header(false);
  auto rows = csv.rows();

  ASSERT_EQ(rows.size(), 2);

  ASSERT_EQ(rows[0]["0"], "1");
  ASSERT_EQ(rows[0]["1"], "2");
  ASSERT_EQ(rows[0]["2"], "3");
  ASSERT_EQ(rows[1]["0"], "4");
  ASSERT_EQ(rows[1]["1"], "5");
  ASSERT_EQ(rows[1]["2"], "6");
}

TEST("read the csv file which has no header but custom config header") {
  CSVReader csv(TEST_FILE("no_header.csv"));
  csv.configureDialect().header(false).column_names("a", "b", "c");
  auto rows = csv.rows();

  ASSERT_EQ(rows.size(), 2);

  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "5");
  ASSERT_EQ(rows[1]["c"], "6");
}

TEST("read the csv file whose field has whitespace") {
  CSVReader csv(TEST_FILE("test_10.csv"));
  csv.configureDialect().delimiter(':').trim_characters(' ', '[', ']');

  auto rows = csv.rows();

  ASSERT_EQ(rows.size(), 2);
  ASSERT_EQ(rows[0]["Timestamp"], "1555164718");
  ASSERT_EQ(rows[0]["Thread ID"], "04");
  ASSERT_EQ(rows[0]["Log Level"], "INFO");
  ASSERT_EQ(rows[0]["Log Message"], "Hello World");
  ASSERT_EQ(rows[1]["Timestamp"], "1555463132");
  ASSERT_EQ(rows[1]["Thread ID"], "02");
  ASSERT_EQ(rows[1]["Log Level"], "DEBUG");
  ASSERT_EQ(rows[1]["Log Message"], "Warning! Foo has happened");
}

TEST("read the excel csv file") {
  CSVReader csv(TEST_FILE("test_11_excel.csv"));

  auto rows = csv.rows();

  ASSERT_EQ(rows.size(), 2);

  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "5");
  ASSERT_EQ(rows[1]["c"], "6");
}

TEST("read headers with double quotes") {
  CSVReader csv(TEST_FILE("test_06.csv"));

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 0);

  auto cols = csv.cols();
  ASSERT_EQ(cols.size(), 3);
  ASSERT_EQ(cols[0], "Free trip to A,B");
  ASSERT_EQ(cols[1], "5.89");
  ASSERT_EQ(cols[2], "Special rate \"1.79\"");
}

TEST("read headers with pairs of single-quotes") {
  CSVReader csv(TEST_FILE("test_07.csv"));
  csv.configureDialect().quote_character('\'');

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 0);
  auto cols = csv.cols();
  ASSERT_EQ(cols.size(), 3);
  ASSERT_EQ(cols[0], "Free trip to A,B");
  ASSERT_EQ(cols[1], "'5.89'");
  ASSERT_EQ(cols[2], "Special rate '1.79'");
}

TEST("read with double quotes which doublequote is false") {
  CSVReader csv(TEST_FILE("test_06.csv"));
  csv.configureDialect().double_quote(false);

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 0);

  auto cols = csv.cols();
  ASSERT_EQ(cols.size(), 3);
  ASSERT_EQ(cols[0], "Free trip to A,B");
  ASSERT_EQ(cols[1], "5.89");
  ASSERT_EQ(cols[2], "Special rate \"1.79\"\"\"");
}

TEST("read file which contains null field enclosed by quote") {
  CSVReader csv(TEST_FILE("test_08.csv"));

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 2);
  ASSERT_EQ(rows[0]["a"], "");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "");
  ASSERT_EQ(rows[1]["c"], "6");
}

TEST("read csv with empty lines and skip empty rows") {
  CSVReader csv(TEST_FILE("empty_lines.csv"));
  csv.configureDialect().skip_empty_rows(true);

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 4);
  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "5");
  ASSERT_EQ(rows[1]["c"], "6");
  ASSERT_EQ(rows[2]["a"], "7");
  ASSERT_EQ(rows[2]["b"], "8");
  ASSERT_EQ(rows[2]["c"], "9");
  ASSERT_EQ(rows[3]["a"], "10");
  ASSERT_EQ(rows[3]["b"], "11");
  ASSERT_EQ(rows[3]["c"], "12");
}

TEST("read csv with empty lines but not skip") {
  CSVReader csv(TEST_FILE("empty_lines.csv"));
  csv.configureDialect().skip_empty_rows(false);

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 7);
  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "5");
  ASSERT_EQ(rows[1]["c"], "6");
  ASSERT_EQ(rows[2]["a"], "7");
  ASSERT_EQ(rows[2]["b"], "8");
  ASSERT_EQ(rows[2]["c"], "9");
  ASSERT_EQ(rows[3]["a"], "");
  ASSERT_EQ(rows[3]["b"], "");
  ASSERT_EQ(rows[3]["c"], "");
  ASSERT_EQ(rows[4]["a"], "10");
  ASSERT_EQ(rows[4]["b"], "11");
  ASSERT_EQ(rows[4]["c"], "12");
  ASSERT_EQ(rows[5]["a"], "");
  ASSERT_EQ(rows[5]["b"], "");
  ASSERT_EQ(rows[5]["c"], "");
  ASSERT_EQ(rows[6]["a"], "");
  ASSERT_EQ(rows[6]["b"], "");
  ASSERT_EQ(rows[6]["c"], "");
}

TEST("read csv with missing columns") {
  CSVReader csv(TEST_FILE("missing_columns.csv"));

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 2);
  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0][""], "");
  ASSERT_EQ(rows[0]["d"], "4");
  ASSERT_EQ(rows[1]["a"], "5");
  ASSERT_EQ(rows[1]["b"], "6");
  ASSERT_EQ(rows[1][""], "");
  ASSERT_EQ(rows[1]["d"], "8");
}

TEST("read csv with missing columns 2") {
  CSVReader csv(TEST_FILE("missing_columns_2.csv"));

  csv.configureDialect().delimiter(';');

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 6);
  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "1");
  ASSERT_EQ(rows[1]["b"], "2");
  ASSERT_EQ(rows[1]["c"], "");
  ASSERT_EQ(rows[2]["a"], "1");
  ASSERT_EQ(rows[2]["b"], "2");
  ASSERT_EQ(rows[2]["c"], "");
  ASSERT_EQ(rows[3]["a"], "1");
  ASSERT_EQ(rows[3]["b"], "");
  ASSERT_EQ(rows[3]["c"], "");
  ASSERT_EQ(rows[4]["a"], "1");
  ASSERT_EQ(rows[4]["b"], "");
  ASSERT_EQ(rows[4]["c"], "");
  ASSERT_EQ(rows[5]["a"], "1");
  ASSERT_EQ(rows[5]["b"], "");
  ASSERT_EQ(rows[5]["c"], "");
}

TEST("read csv with too many columns") {
  CSVReader csv(TEST_FILE("too_many_columns.csv"));

  csv.configureDialect().delimiter(',').trim_characters(' ');

  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 2);
  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "6");
  ASSERT_EQ(rows[1]["b"], "7");
  ASSERT_EQ(rows[1]["c"], "");
}

TEST("read csv whose field contains delimeter") {
  CSVReader csv(TEST_FILE("contains_comma.csv"));
  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 1);
  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2,");
  ASSERT_EQ(rows[0]["c"], "3");
}

TEST("read csv whose field contains line terminator") {
  CSVReader csv(TEST_FILE("contains_terminator.csv"));
  auto rows = csv.rows();
  ASSERT_EQ(rows.size(), 1);
  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2,\n4,\n5");
  ASSERT_EQ(rows[0]["c"], "3");
}

TEST("read a csv where double quotes do not match") {
  CSVReader csv(TEST_FILE("bad.csv"));

  bool exception_thrown = false;
  try {
    auto rows = csv.rows();
  } catch (std::exception&) {
    exception_thrown = true;
  }
  ASSERT_TRUE(exception_thrown);
}

TEST("read a csv where double quotes do not match but doublequote is false") {
  CSVReader csv(TEST_FILE("bad.csv"));
  csv.configureDialect().double_quote(false);

  auto rows = csv.rows();
  ASSERT_EQ(rows[0]["a"], "1\"");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
}

TEST("read a csv which use CRLF as line terminator") {
  CSVReader csv(TEST_FILE("contains_CRLF.csv"));

  auto rows = csv.rows();
  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
  ASSERT_EQ(rows[1]["a"], "4");
  ASSERT_EQ(rows[1]["b"], "5");
  ASSERT_EQ(rows[1]["c"], "6");
}
}
;

}  // namespace cub
