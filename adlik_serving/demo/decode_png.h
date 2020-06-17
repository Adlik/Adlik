// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef ADLIK_SERVING_DEMO_DECODE_PNG_H
#define ADLIK_SERVING_DEMO_DECODE_PNG_H

#include "adlik_serving/demo/image.h"

namespace adlik {
namespace serving {
namespace demo {
Image decode_png(const char* file_path, size_t channels);
}  // namespace demo
}  // namespace serving
}  // namespace adlik

#endif  // ADLIK_SERVING_DEMO_DECODE_PNG_H
