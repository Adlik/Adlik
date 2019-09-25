#include "cub/env/env.h"
#include "cub/env/concurrent/concurrent.h"
#include "cub/env/fs/file_system.h"
#include "cub/env/time/time.h"

namespace cub {

namespace {
struct WindowsConcurrent : Concurrent {
private:
  OVERRIDE(Thread* start(thread_t)) {
    return nullptr;
  }

  OVERRIDE(void sched(thread_t)) {
  }

  OVERRIDE(void schedAfter(int64_t micros, thread_t)) {
  }
};

struct WindowsTime : Time {
private:
  OVERRIDE(uint64_t nowNanos() const) {
    return 0;
  }

  OVERRIDE(void sleep(uint64_t micros) const) {
  }
};

struct WindowsFileSystem : FileSystem {
private:
  OVERRIDE(ReadOnlyRegion* mmap(const std::string& fname) const) {
    return nullptr;
  }

  OVERRIDE(Status children(const std::string& dir, DirentVisitor& visitor)
               const) {
    return cub::Failure;
  }

  OVERRIDE(bool exists(const std::string& fname) const) {
    return false;
  }
};

struct WindowsEnv : Env,
                    private WindowsConcurrent,
                    private WindowsTime,
                    private WindowsFileSystem {
private:
  IMPL_ROLE(Time);
  IMPL_ROLE(Concurrent);
  IMPL_ROLE(FileSystem);
};
}  // namespace

Env& Env::inst() {
  static WindowsEnv env;
  return env;
}

}  // namespace cub
