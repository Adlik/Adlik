// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/env/concurrent/loop_thread.h"

#include <memory>
#include <string>

#include "cub/env/env.h"
#include "cub/env/time/time.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

FIXTURE(LoopThreadTest){TEST("zero interval"){LoopThread loop([]() { timesystem().sleep(20 * 1000); }, 0);
}
}
;
}
