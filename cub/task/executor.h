#ifndef H26AC534F_0E33_4F66_8BDC_191637618930
#define H26AC534F_0E33_4F66_8BDC_191637618930

#include <functional>
#include "cub/dci/role.h"
#include "cub/env/concurrent/thread.h"

namespace cub {

DEFINE_ROLE(Executor) {
  ABSTRACT(void schedule(thread_t));
};

}  // namespace cub

#endif
