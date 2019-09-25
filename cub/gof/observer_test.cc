#include "cub/gof/observer.h"
#include "cub/env/env.h"
#include "cub/env/time/time.h"
#include "cub/env/concurrent/loop_thread.h"
#include "cut/cut.hpp"

using namespace cum;

namespace cub {

FIXTURE(ObserverTest) {
  TEST("Call") {
    int num_calls = 0;
    Observer<> observer([&]() { num_calls++; });
    observer.notifier()();
    ASSERT_EQ(1, num_calls);
  }

  TEST("CallWithArg") {
    Observer<int> observer([&](int arg) { ASSERT_EQ(1337, arg); });
    observer.notifier()(1337);
  }

  TEST("Orphan") {
    int num_calls = 0;
    std::function<void()> notifier;
    {
      Observer<> observer([&]() { num_calls++; });
      notifier = observer.notifier();
      ASSERT_EQ(0, num_calls);
      notifier();
      ASSERT_EQ(1, num_calls);
    }
    notifier();
    ASSERT_EQ(1, num_calls);
  }

  TEST("multi threads") {
    std::mutex mu;
    int num_calls = 0;
    auto observer = std::unique_ptr<Observer<>>(
        new Observer<>([&mu, &num_calls]() {
          std::lock_guard<std::mutex> l(mu);
          ++num_calls;
        })
    );
    auto notifier = observer->notifier();

    // Spawn a thread and wait for it to run a few times.
    LoopThread thread(notifier, 1000 /* 1 milliseconds */);
    while (true) {
      {
        std::lock_guard<std::mutex> l(mu);
        if (num_calls >= 10) {
          break;
        }
      }
      Env::inst().ROLE(Time).sleep(1000 /* 1 milliseconds */);
    }

    // Tear down the observer and make sure it is never called again.
    observer = nullptr;
    int num_calls_snapshot;
    {
      std::lock_guard<std::mutex> l(mu);
      num_calls_snapshot = num_calls;
    }
    Env::inst().ROLE(Time).sleep(100 * 1000 /* 100 milliseconds */);
    {
      std::lock_guard<std::mutex> l(mu);
      ASSERT_EQ(num_calls_snapshot, num_calls);
    }
  }
};

}
