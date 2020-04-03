// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_CELL_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_CELL_H

#include <ostream>
#include <string>

#include "adlik_serving/runtime/ml/algorithm/grid/types.h"

namespace ml_runtime {

struct Cell {
  PLMN plmn;
  NBId nb;
  CellId cell;

  std::string toString() const;
};

bool operator==(const Cell&, const Cell&);
bool operator!=(const Cell&, const Cell&);

std::ostream& operator<<(std::ostream&, const Cell&);

}  // namespace ml_runtime

#endif
