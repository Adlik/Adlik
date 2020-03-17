// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/grid/csv_reader.h"

#include "cut/cut.hpp"
#include <iostream>

using namespace cum;
using namespace ml_runtime;

FIXTURE(CsvReaderTest){TEST("read a simple csv"){CSVReader reader(
    "/home/jsy/ai/code/Adlik/adlik_serving/runtime/ml/algorithm/grid/test_data/172759_3_T054Clean_Con_kmeans.csv");
reader.read();
auto cols = reader.col_names();
std::cout << "cols size: " << cols.size() << std::endl;
for (auto it : cols) {
  std::cout << '[' << it << "]\n";
}
std::cout << std::endl;

size_t lines = 0;
reader.getData([&](const CSVReader::Row&) {
  lines++;
  return true;
});

std::cout << "lines: " << lines << std::endl;
}
}
;
