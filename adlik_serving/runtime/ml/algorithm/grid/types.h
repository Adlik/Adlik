// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_TYPES_H
#define ADLIK_SERVING_RUNTIME_ML_ALGORITHM_GRID_TYPES_H

#include <climits>

namespace ml_runtime {

typedef unsigned long PLMN;
typedef unsigned int NBId;
typedef unsigned short CellId;

typedef short Rsrp;
typedef unsigned char Event;

const Rsrp MAX_RSRP = SHRT_MAX;
const Rsrp MIN_RSRP = SHRT_MIN;

}  // namespace ml_runtime

#endif
