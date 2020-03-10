#pragma once
#include <catch.hpp>
#include <csv/reader.hpp>
#include <csv/writer.hpp>

TEST_CASE("Parse an empty CSV", "[simple csv]") {
  csv::Reader csv;
  csv.read("inputs/empty.csv");
  auto rows = csv.rows();
  auto cols = csv.cols();
  REQUIRE(rows.size() == 0);
  REQUIRE(cols.size() == 0);
}

TEST_CASE("Parse file that doesn't exist", "[simple csv]") {
  csv::Reader csv;
  bool exception_thrown = false;
  try {
    csv.read("inputs/missing.csv");
  }
  catch (std::exception&) {
    exception_thrown = true;
  }
  REQUIRE(exception_thrown);
}

TEST_CASE("Parse the most basic of CSV buffers", "[simple csv]") {
  csv::Reader csv;
  csv.read("inputs/test_01.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
}

TEST_CASE("Parse the most basic of CSV buffers (Iterator)", "[simple csv]") {
  csv::Reader csv;
  csv.read("inputs/test_01.csv");
  std::vector<csv::unordered_flat_map<std::string_view, std::string>> rows;
  while (csv.busy()) {
    if (csv.ready()) {
      auto row = csv.next_row();
      rows.push_back(row);
    }
  }

  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
}

TEST_CASE("Parse the most basic of CSV buffers with ', ' delimiter", "[simple csv]") {
  csv::Reader csv;
  auto foo = csv.configure_dialect("test_dialect");
  csv.configure_dialect("test_dialect")
    .delimiter(", ");
  csv.read("inputs/test_02.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
}

TEST_CASE("Parse the most basic of CSV buffers with ', ' delimiter using skip_initial_space_", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect("test_dialect")
    .delimiter(", ")
    .skip_initial_space(true);
  csv.read("inputs/test_02.csv");

  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
}

TEST_CASE("Parse the most basic of CSV buffers with '::' delimiter", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect("test_dialect")
    .delimiter("::");
  csv.read("inputs/test_03.csv");

  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
}

TEST_CASE("Parse the most basic of CSV buffers - Trim whitespace characters", "[simple csv]") {
  csv::Reader csv;

  csv.configure_dialect("test_dialect")
    .trim_characters(' ', '\t');
  csv.read("inputs/test_02.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
}

TEST_CASE("Parse the most basic of CSV buffers - Trim whitespace characters gone crazy", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect("test_dialect")
    .trim_characters(' ', '\t');
  csv.read("inputs/test_04.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
}

TEST_CASE("Parse the most basic of CSV buffers - Log messages", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect("test_dialect")
    .delimiter("::");
  csv.read("inputs/test_05.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 3);
  REQUIRE(rows[0]["Thread_ID"] == "1");
  REQUIRE(rows[0]["Log_Level"] == "DEBUG");
  REQUIRE(rows[0]["Message"] == "Thread Started");
  REQUIRE(rows[1]["Thread_ID"] == "2");
  REQUIRE(rows[1]["Log_Level"] == "DEBUG");
  REQUIRE(rows[1]["Message"] == "Thread Started");
  REQUIRE(rows[2]["Thread_ID"] == "3");
  REQUIRE(rows[2]["Log_Level"] == "ERROR");
  REQUIRE(rows[2]["Message"] == "File not found");
}

TEST_CASE("Parse the most basic of CSV buffers - No header row", "[simple csv]") {
  csv::Reader csv;

  csv.configure_dialect("test_dialect")
    .header(false);
  csv.read("inputs/test_08.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 3);
  REQUIRE(rows[0]["0"] == "1");
  REQUIRE(rows[0]["1"] == "2");
  REQUIRE(rows[0]["2"] == "3");
  REQUIRE(rows[1]["0"] == "4");
  REQUIRE(rows[1]["1"] == "5");
  REQUIRE(rows[1]["2"] == "6");
  REQUIRE(rows[2]["0"] == "7");
  REQUIRE(rows[2]["1"] == "8");
  REQUIRE(rows[2]["2"] == "9");
}

TEST_CASE("Parse the most basic of CSV buffers - No header / custom column names", "[simple csv]") {
  csv::Reader csv;

  csv.configure_dialect("test_dialect")
    .header(false)
    .column_names("a", "b", "c");
  csv.read("inputs/test_08.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 3);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
  REQUIRE(rows[2]["a"] == "7");
  REQUIRE(rows[2]["b"] == "8");
  REQUIRE(rows[2]["c"] == "9");
}

TEST_CASE("Parse the most basic of CSV buffers - Space delimiter", "[simple csv]") {
  csv::Reader csv;

  csv.configure_dialect("test_dialect")
    .delimiter(" ");
  csv.read("inputs/test_09.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["first_name"] == "Eric");
  REQUIRE(rows[0]["last_name"] == "Idle");
  REQUIRE(rows[1]["first_name"] == "John");
  REQUIRE(rows[1]["last_name"] == "Cleese");
}

TEST_CASE("Parse the most basic of CSV buffers - Log with header", "[simple csv]") {
  csv::Reader csv;

  csv.configure_dialect("test_dialect")
    .delimiter("::")
    .trim_characters(' ', '[', ']');
  csv.read("inputs/test_10.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["Timestamp"] == "1555164718");
  REQUIRE(rows[0]["Thread ID"] == "04");
  REQUIRE(rows[0]["Log Level"] == "INFO");
  REQUIRE(rows[0]["Log Message"] == "Hello World");
  REQUIRE(rows[1]["Timestamp"] == "1555463132");
  REQUIRE(rows[1]["Thread ID"] == "02");
  REQUIRE(rows[1]["Log Level"] == "DEBUG");
  REQUIRE(rows[1]["Log Message"] == "Warning! Foo has happened");
}

TEST_CASE("Parse Excel CSV", "[simple csv]") {
  csv::Reader csv;
  csv.use_dialect("excel");
  csv.read("inputs/test_11_excel.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
}

TEST_CASE("Parse Unix CSV", "[simple csv]") {
  csv::Reader csv;
  csv.use_dialect("unix");
  csv.read("inputs/test_12_unix.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
}

TEST_CASE("Parse messed up log files", "[simple csv]") {
  csv::Reader csv;

  csv.configure_dialect("my strange dialect")
    .delimiter("::")
    .trim_characters(' ', '[', ']', '{', '}');
  csv.read("inputs/test_13.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["Timestamp"] == "1555164718");
  REQUIRE(rows[0]["Thread ID"] == "04");
  REQUIRE(rows[0]["Log Level"] == "INFO");
  REQUIRE(rows[0]["Log Message"] == "Hello World");
  REQUIRE(rows[1]["Timestamp"] == "1555463132");
  REQUIRE(rows[1]["Thread ID"] == "02");
  REQUIRE(rows[1]["Log Level"] == "DEBUG");
  REQUIRE(rows[1]["Log Message"] == "Warning! Foo has happened");
}

TEST_CASE("Parse headers with double quotes", "[simple csv]") {
  csv::Reader csv;
  csv.read("inputs/test_06.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 0);
  auto cols = csv.cols();
  REQUIRE(cols.size() == 3);
  REQUIRE(cols[0] == "\"Free trip to A,B\"");
  REQUIRE(cols[1] == "\"5.89\"");
  REQUIRE(cols[2] == "\"Special rate \"\"1.79\"\"\"");
}

TEST_CASE("Parse headers with pairs of single-quotes", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect("test_dialect")
    .quote_character('\'');
  csv.read("inputs/test_07.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 0);
  auto cols = csv.cols();
  REQUIRE(cols.size() == 3);
  REQUIRE(cols[0] == "''Free trip to A,B''");
  REQUIRE(cols[1] == "''5.89''");
  REQUIRE(cols[2] == "''Special rate ''''1.79''''''");
}

TEST_CASE("Parse the most basic of CSV buffers and ignore 1 column", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect("test_dialect")
    .ignore_columns("a");
  csv.read("inputs/test_01.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0].count("a") == 0);
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1].count("a") == 0);
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
}

TEST_CASE("Parse the most basic of CSV buffers and ignore 2 columns", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect("test_dialect")
    .ignore_columns("a", "b");
  csv.read("inputs/test_01.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0].count("a") == 0);
  REQUIRE(rows[0].count("b") == 0);
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1].count("a") == 0);
  REQUIRE(rows[1].count("b") == 0);
  REQUIRE(rows[1]["c"] == "6");
}

TEST_CASE("Parse the most basic of CSV buffers and ignore all columns", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect("test_dialect")
    .ignore_columns("a", "b", "c");
  csv.read("inputs/test_01.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0].size() == 0);
  REQUIRE(rows[1].size() == 0);
}

TEST_CASE("Parse the most basic of CSV buffers and ignore age/gender columns", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect("test_dialect")
    .delimiter(", ")
    .ignore_columns("age", "gender");
  csv.read("inputs/test_14.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 3);
  REQUIRE(rows[0]["name"] == "Mark Johnson");
  REQUIRE(rows[0]["email"] == "mark.johnson@gmail.com");
  REQUIRE(rows[0]["department"] == "BA");
  REQUIRE(rows[0].count("age") == 0);
  REQUIRE(rows[0].count("gender") == 0);
  REQUIRE(rows[1]["name"] == "John Stevenson");
  REQUIRE(rows[1]["email"] == "john.stevenson@gmail.com");
  REQUIRE(rows[1]["department"] == "IT");
  REQUIRE(rows[1].count("age") == 0);
  REQUIRE(rows[1].count("gender") == 0);
  REQUIRE(rows[2]["name"] == "Jane Barkley");
  REQUIRE(rows[2]["email"] == "jane.barkley@gmail.com");
  REQUIRE(rows[2]["department"] == "MGT");
  REQUIRE(rows[2].count("age") == 0);
  REQUIRE(rows[2].count("gender") == 0);
}

TEST_CASE("Parse CSV with empty lines", "[simple csv]") {
  csv::Reader csv;
  csv.read("inputs/empty_lines.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 7);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
  REQUIRE(rows[2]["a"] == "7");
  REQUIRE(rows[2]["b"] == "8");
  REQUIRE(rows[2]["c"] == "9");
  REQUIRE(rows[3]["a"] == "");
  REQUIRE(rows[3]["b"] == "");
  REQUIRE(rows[3]["c"] == "");
  REQUIRE(rows[4]["a"] == "10");
  REQUIRE(rows[4]["b"] == "11");
  REQUIRE(rows[4]["c"] == "12");
  REQUIRE(rows[5]["a"] == "");
  REQUIRE(rows[5]["b"] == "");
  REQUIRE(rows[5]["c"] == "");
  REQUIRE(rows[6]["a"] == "");
  REQUIRE(rows[6]["b"] == "");
  REQUIRE(rows[6]["c"] == "");
}

TEST_CASE("Parse CSV with empty lines - skip empty rows", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect()
    .skip_empty_rows(true);

  csv.read("inputs/empty_lines.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 4);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "4");
  REQUIRE(rows[1]["b"] == "5");
  REQUIRE(rows[1]["c"] == "6");
  REQUIRE(rows[2]["a"] == "7");
  REQUIRE(rows[2]["b"] == "8");
  REQUIRE(rows[2]["c"] == "9");
  REQUIRE(rows[3]["a"] == "10");
  REQUIRE(rows[3]["b"] == "11");
  REQUIRE(rows[3]["c"] == "12");
}

TEST_CASE("Parse CSV with missing columns", "[simple csv]") {
  csv::Reader csv;

  csv.read("inputs/missing_columns.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0][""] == "");
  REQUIRE(rows[0]["d"] == "4");
  REQUIRE(rows[1]["a"] == "5");
  REQUIRE(rows[1]["b"] == "6");
  REQUIRE(rows[1][""] == "");
  REQUIRE(rows[1]["d"] == "8");
}

TEST_CASE("Parse CSV with missing columns II", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect()
    .delimiter(";");
  csv.read("inputs/missing_columns_2.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 6);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "1");
  REQUIRE(rows[1]["b"] == "2");
  REQUIRE(rows[1]["c"] == "");
  REQUIRE(rows[2]["a"] == "1");
  REQUIRE(rows[2]["b"] == "2");
  REQUIRE(rows[2]["c"] == "");
  REQUIRE(rows[3]["a"] == "1");
  REQUIRE(rows[3]["b"] == "");
  REQUIRE(rows[3]["c"] == "");
  REQUIRE(rows[4]["a"] == "1");
  REQUIRE(rows[4]["b"] == "");
  REQUIRE(rows[4]["c"] == "");
  REQUIRE(rows[5]["a"] == "1");
  REQUIRE(rows[5]["b"] == "");
  REQUIRE(rows[5]["c"] == "");
}

TEST_CASE("Parse CSV with too many columns", "[simple csv]") {
  csv::Reader csv;
  csv.configure_dialect()
    .delimiter(",")
    .trim_characters(' ');
  csv.read("inputs/too_many_columns.csv");
  auto rows = csv.rows();
  REQUIRE(rows.size() == 2);
  REQUIRE(rows[0]["a"] == "1");
  REQUIRE(rows[0]["b"] == "2");
  REQUIRE(rows[0]["c"] == "3");
  REQUIRE(rows[1]["a"] == "6");
  REQUIRE(rows[1]["b"] == "7");
  REQUIRE(rows[1]["c"] == "");
}