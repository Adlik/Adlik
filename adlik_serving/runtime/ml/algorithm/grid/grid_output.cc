// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/grid/grid_output.h"

#include "csv/writer.hpp"
#include "cub/env/fs/file_system.h"
#include "cub/env/fs/path.h"

namespace ml_runtime {

namespace {

static std::vector<std::string> HALF_HEADER = {"ServerRSRP_Core",
                                               "ServerRSRP_MAX",
                                               "ServerRSRP_MIN",

                                               "NeighPLMN_intra1",
                                               "NeighgNB_intra1",
                                               "NeighCID_intra1",
                                               "NeighCellRSRP1_intra_Core",
                                               "NeighCellRSRP1_intra_MAX",
                                               "NeighCellRSRP1_intra_MIN",

                                               "NeighPLMN_intra2",
                                               "NeighgNB_intra2",
                                               "NeighCID_intra2",
                                               "NeighCellRSRP2_intra_Core",
                                               "NeighCellRSRP2_intra_MAX",
                                               "NeighCellRSRP2_intra_MIN",

                                               "NeighCellNum_intra"};

static std::vector<std::string> neighbor_header = {"NeighPLMN_intra_Sta",
                                                   "NeighgNB_intra_Sta",
                                                   "NeighCID_intra_Sta",
                                                   "Event1Num_Sta",
                                                   "Event2Num_Sta",
                                                   "Event3Num_Sta"};

size_t MAX_NEIGHBOR_NUM = 10;

struct Saver : GridCsvSaver {
  Saver(const std::string& path) : writer(path) {
    saveHeader();
  }

  ~Saver() {
    writer.close();
  }

  Saver(Saver&&) = delete;
  Saver(const Saver&) = delete;

  bool save(const GridResult& output) {
    std::vector<std::string> row;
    row.push_back(std::to_string(output.serverRSRP_core));
    row.push_back(std::to_string(output.serverRSRP_max));
    row.push_back(std::to_string(output.serverRSRP_min));

    row.push_back(std::to_string(output.neighbor1.plmn));
    row.push_back(std::to_string(output.neighbor1.nb));
    row.push_back(std::to_string(output.neighbor1.cell));
    row.push_back(std::to_string(output.neighRSRP_core1));
    row.push_back(std::to_string(output.neighRSRP_max1));
    row.push_back(std::to_string(output.neighRSRP_min1));

    row.push_back(std::to_string(output.neighbor2.plmn));
    row.push_back(std::to_string(output.neighbor2.nb));
    row.push_back(std::to_string(output.neighbor2.cell));
    row.push_back(std::to_string(output.neighRSRP_core2));
    row.push_back(std::to_string(output.neighRSRP_max2));
    row.push_back(std::to_string(output.neighRSRP_min2));

    row.push_back(std::to_string(output.neighbor_num));
    for (size_t i = 0; i < std::min(output.neighbor_num, MAX_NEIGHBOR_NUM); ++i) {
      row.push_back(std::to_string(output.stats[i].neighbor.plmn));
      row.push_back(std::to_string(output.stats[i].neighbor.nb));
      row.push_back(std::to_string(output.stats[i].neighbor.cell));
      row.push_back(std::to_string(output.stats[i].event_1));
      row.push_back(std::to_string(output.stats[i].event_2));
      row.push_back(std::to_string(output.stats[i].event_3));
    }

    writer.write_row(row);
    return true;
  }

private:
  void saveHeader() {
    std::vector<std::string> header(HALF_HEADER);
    for (size_t i = 1; i < MAX_NEIGHBOR_NUM + 1; ++i) {
      for (const auto& h : neighbor_header) {  // todo: will be optimized by std::transform in c++17 & c++20
        header.push_back(h + std::to_string(i));
      }
    }
    writer.write_row(header);
  }

  csv::Writer writer;
};

}  // namespace

size_t GridResult::maxNeighborNum() {
  return MAX_NEIGHBOR_NUM;
}

cub::StatusWrapper GridCsvSaver::create(const std::string& file_path, std::unique_ptr<GridCsvSaver>* saver) {
  auto path = cub::Path(file_path);
  auto dir = path.dirName();
  if (!cub::filesystem().exists(dir.to_s())) {
    return cub::StatusWrapper(cub::InvalidArgument, "Output directory doesn't exist");
  }
  *saver = std::make_unique<Saver>(file_path);
  return cub::StatusWrapper::OK();
}

bool GridCsvSaver::save(const std::vector<GridResult>& outputs) {
  for (const auto& o : outputs) {
    save(o);
  }
  return true;
}

}  // namespace ml_runtime
