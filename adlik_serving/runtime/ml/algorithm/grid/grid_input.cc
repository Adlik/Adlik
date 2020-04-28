// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "adlik_serving/runtime/ml/algorithm/grid/grid_input.h"

#include <algorithm>

#include "cub/csv/csv_reader.h"
#include "cub/log/log.h"
#include "cub/string/str_utils.h"

namespace ml_runtime {

namespace {

static std::vector<std::string> expected_cols = {"ServerRSRP",
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
  try {
    cub::CSVReader reader(file_path);

    auto col_names = reader.cols();
    for (const auto& i : expected_cols) {
      if (std::find(col_names.begin(), col_names.end(), i) == col_names.end()) {
        ERR_LOG << "csv headers is not equal to expected!";
        DEBUG_LOG << "Expected columns is: " << cub::strutils::join(expected_cols, ",");
        return cub::StatusWrapper(cub::Internal, "csv headers is not equal to expected!");
      }
    }

    inputs.clear();
    auto func = [&inputs](const cub::CSVReader::Row& row) {
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

    cub::CSVReader::Row row;
    while (reader.nextRow(row)) {
      if (!func(row)) {
        return cub::StatusWrapper(cub::Internal, "csv field to struct failure");
      }
    }
    return cub::StatusWrapper::OK();
  } catch (const std::runtime_error& e) {
    FATAL_LOG << "Runtime error: " << e.what();
  } catch (const std::exception& e) {
    ERR_LOG << "Exception: " << e.what();
  }

  return cub::StatusWrapper(cub::Internal, "exception when read csv file");
}

}  // namespace ml_runtime
