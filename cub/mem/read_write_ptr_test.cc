#include "cub/mem/read_write_ptr.h"
#include "cub/env/env.h"
#include "cub/env/concurrent/concurrent.h"
#include "cut/cut.hpp"

#include <vector>

using namespace cum;

FIXTURE(ReadWritePtrOnSingleThreadTest) {
  cub::ReadWritePtr<int> ptr;

  TEST("initially the object should be null") {
    auto pointer = ptr.get();
    ASSERT_THAT(pointer == nullptr, be_true());
  }

  TEST("swap") {
    ASSERT_THAT(ptr.update(std::make_unique<int>(1)) == nullptr, be_true());
    auto pointer = ptr.get();
    ASSERT_THAT(*pointer, eq(1));
  }
};

FIXTURE(ReadWritePtrOnMultiThreadTest) {
  enum { NUM_THREADS = 4, MAX_NUM = 10000 };

  cub::ReadWritePtr<int> ptr = std::make_unique<int>(0);

  TEST("multi threads") {
    std::vector<std::unique_ptr<cub::Thread>> threads;
    for (auto tid = 0; tid != NUM_THREADS; ++tid) {
      threads.emplace_back(thread(tid));
    }
  }

  cub::Thread* thread(int tid) {
    return cub::concurrent().start([tid, this]() {
      while (loop(tid))
        ;
    });
  }

  bool loop(int tid) {
    auto value = read();
    return value != MAX_NUM ? write(value, tid) : false;
  }

  int read() const {
    auto pointer = ptr.get();
    ASSERT_THAT(*pointer, ge(-1));
    return *pointer;
  }

  bool write(int value, int tid) {
    if (value % NUM_THREADS == tid) {
      ptr.update(std::make_unique<int>(value + 1));
    }
    return true;
  }
};
