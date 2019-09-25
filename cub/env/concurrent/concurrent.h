#ifndef H0F166B02_2E75_43A3_9C1E_4BF6E8D7B586
#define H0F166B02_2E75_43A3_9C1E_4BF6E8D7B586

#include <stdint.h>
#include "cub/dci/role.h"
#include "cub/env/concurrent/thread.h"
#include "cub/env/env.h"

namespace cub {

DEFINE_ROLE(Concurrent) {
  ABSTRACT(Thread * start(thread_t));
  ABSTRACT(void sched(thread_t));
  ABSTRACT(void schedAfter(int64_t micros, thread_t));
};

inline Concurrent& concurrent() {
  return Env::inst().ROLE(Concurrent);
}

}  // namespace cub

#endif
