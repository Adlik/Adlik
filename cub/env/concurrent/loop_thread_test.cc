#include "cub/env/concurrent/loop_thread.h"
#include "cub/env/time/time.h"
#include "cub/env/env.h"
#include "cut/cut.hpp"

#include <memory>
#include <string>

using namespace cum;

namespace cub {

FIXTURE(LoopThreadTest) {
  TEST("zero interval") {
    LoopThread loop([]() {
      timesystem().sleep(20 * 1000);
    }, 0);
  }
};

}
