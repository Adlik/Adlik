#include "cub/env/concurrent/thread_pool.h"
#include "cub/env/concurrent/concurrent.h"
#include "cub/env/env.h"

#define EIGEN_USE_THREADS
#include "third_party/eigen3/unsupported/Eigen/CXX11/ThreadPool.h"

namespace cub {

namespace {
struct EigenEnv {
  using EnvThread = Thread;

  struct Task {
    thread_t f;
  };

  Thread* CreateThread(thread_t f) {
    return concurrent().start(std::move(f));
  }

  Task CreateTask(thread_t f) {
    return {std::move(f)};
  }

  void ExecuteTask(const Task& t) {
    t.f();
  }
};

using EigenTheadPool = Eigen::ThreadPoolTempl<EigenEnv>;
}  // namespace

struct ThreadPool::Impl : EigenTheadPool {
  Impl(int numThreads) : EigenTheadPool(numThreads, true, EigenEnv()) {
  }
};

ThreadPool::ThreadPool(int numThreads)
  : impl(new ThreadPool::Impl(numThreads)) {
}

ThreadPool::~ThreadPool() {
  delete impl;
}

void ThreadPool::schedule(thread_t fn) {
  impl->Schedule(std::move(fn));
}

void ThreadPool::cancel() {
  impl->Cancel();
}

int ThreadPool::getNumThreads() const {
  return impl->NumThreads();
}

int ThreadPool::getCurrentThreadId() const {
  return impl->CurrentThreadId();
}

}  // namespace cub
