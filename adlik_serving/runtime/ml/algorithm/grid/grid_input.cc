// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/grid/grid_input.h"

#include "adlik_serving/runtime/ml/algorithm/grid/csv_reader.h"
#include "cub/env/fs/file_system.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"

namespace ml_runtime {

namespace {

static std::vector<std::string> expected_cols = {"ServergPLMN",
                                                 "ServergNB",
                                                 "ServerCID",
                                                 "ServerRSRP",
                                                 "NeighPLMN_intra1",
                                                 "NeighgNB_intra1",
                                                 "NeighCID_intra1",
                                                 "NeighCellRSRP1_intra",
                                                 "NeighPLMN_intra2",
                                                 "NeighgNB_intra2",
                                                 "NeighCID_intra2",
                                                 "NeighCellRSRP2_intra",
                                                 "NeighPLMN_intra3",
                                                 "NeighgNB_intra3",
                                                 "NeighCID_intra3",
                                                 "Event"};

#define COL_TO_FIELD(col, field, Type, func)                                                 \
  {                                                                                          \
    auto search = row.find(col);                                                             \
    if (search != row.end()) {                                                               \
      input.field = search->second.length() > 0 ? (Type)std::func(search->second) : (Type)0; \
    } else {                                                                                 \
      return false;                                                                          \
    }                                                                                        \
  }  // namespace

#define COL_TO_FIELD_PLMN(col, field) COL_TO_FIELD(col, field, PLMN, stoul)
#define COL_TO_FIELD_NB(col, field) COL_TO_FIELD(col, field, NBId, stoi)
#define COL_TO_FIELD_CELL(col, field) COL_TO_FIELD(col, field, CellId, stoi)
#define COL_TO_FIELD_RSRP(col, field) COL_TO_FIELD(col, field, Rsrp, stoi)
#define COL_TO_FIELD_EVENT(col, field) COL_TO_FIELD(col, field, Event, stoi)

}  // namespace

//////////////////////////////////////////////////////////////////////////////////////////////
cub::StatusWrapper loadGridInput(const std::string& file_path, std::vector<GridInput>& inputs) {
  if (!cub::filesystem().exists(file_path)) {
    return cub::StatusWrapper(cub::InvalidArgument, "Input file doesn't exist");
  }

  CSVReader reader(file_path);
  if (!reader.read()) {
    ERR_LOG << "Read input csv failure!";
    return cub::StatusWrapper(cub::Internal, "Read input csv failure!");
  }

  auto col_names = reader.col_names();
  if (expected_cols != col_names) {
    ERR_LOG << "csv headers is not equal to expected!";
    for (auto& n : col_names) {
      DEBUG_LOG << '[' << n << ']';
    }
    return cub::StatusWrapper(cub::Internal, "csv headers is not equal to expected!");
  }

  inputs.clear();
  auto func = [&](const CSVReader::Row& row) {
    GridInput input;

    COL_TO_FIELD_RSRP("ServerRSRP", serving_rsrp);

    COL_TO_FIELD_PLMN("NeighPLMN_intra1", neighbor1.plmn);
    COL_TO_FIELD_NB("NeighgNB_intra1", neighbor1.nb);
    COL_TO_FIELD_CELL("NeighCID_intra1", neighbor1.cell);
    COL_TO_FIELD_RSRP("NeighCellRSRP1_intra", neighRSRP_intra1);

    COL_TO_FIELD_PLMN("NeighPLMN_intra2", neighbor2.plmn);
    COL_TO_FIELD_NB("NeighgNB_intra2", neighbor2.nb);
    COL_TO_FIELD_CELL("NeighCID_intra2", neighbor2.cell);
    COL_TO_FIELD_RSRP("NeighCellRSRP2_intra", neighRSRP_intra2);

    COL_TO_FIELD_PLMN("NeighPLMN_intra3", neighbor3.plmn);
    COL_TO_FIELD_NB("NeighgNB_intra3", neighbor3.nb);
    COL_TO_FIELD_CELL("NeighCID_intra3", neighbor3.cell);
    COL_TO_FIELD_EVENT("Event", event);

    inputs.push_back(std::move(input));
    return true;
  };

  return reader.getData(func) ? cub::StatusWrapper::OK() : cub::StatusWrapper(cub::Internal, "Read input failure!");
}

}  // namespace ml_runtime
