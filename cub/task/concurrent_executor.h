#ifndef HF1AC3A5A_490F_494D_81F3_F3F4282048BA
#define HF1AC3A5A_490F_494D_81F3_F3F4282048BA

#include "cub/env/concurrent/thread_pool.h"
#include "cub/task/executor.h"

namespace cub {

struct ConcurrentExecutor : Executor {
  ConcurrentExecutor(int num_threads);
  void schedule(thread_t f) override;

private:
  ThreadPool pool;
};

}  // namespace cub

#endif
