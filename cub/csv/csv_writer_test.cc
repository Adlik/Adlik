// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/csv/csv_writer.h"

#include "cub/csv/csv_reader.h"
#include "cub/csv/test_util.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

FIXTURE(CSVWritererTest){

    TEST("writer csv with header"){auto file_path = TEST_FILE("output_01.csv");
{
  CSVWriter csv(file_path);
  csv.configureDialect().column_names({"a", "b", "c"});
  csv.writeRow({"1", "2", "3"});
}
{
  CSVReader csv(file_path);
  auto rows = csv.rows();
  ASSERT_EQ(1, rows.size());
  ASSERT_EQ(rows[0]["a"], "1");
  ASSERT_EQ(rows[0]["b"], "2");
  ASSERT_EQ(rows[0]["c"], "3");
}
}  // namespace cub

TEST("writer csv with no header") {
  auto file_path = TEST_FILE("output_02.csv");
  {
    CSVWriter csv(file_path);
    csv.writeRow({"1", "2", "3"});
  }
  {
    CSVReader csv(file_path);
    csv.configureDialect().header(false);
    auto rows = csv.rows();
    ASSERT_EQ(1, rows.size());
    ASSERT_EQ(rows[0]["0"], "1");
    ASSERT_EQ(rows[0]["1"], "2");
    ASSERT_EQ(rows[0]["2"], "3");
  }

}  // namespace cub
}
;
}  // namespace cub
