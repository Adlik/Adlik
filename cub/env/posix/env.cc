#include "cub/env/env.h"
#include "cub/env/posix/posix_concurrent.h"
#include "cub/env/posix/posix_filesystem.h"
#include "cub/env/posix/posix_time.h"

namespace cub {

namespace {
struct PosixEnv : Env,
                  private PosixConcurrent,
                  private PosixTime,
                  private PosixFileSystem {
private:
  IMPL_ROLE(Time);
  IMPL_ROLE(Concurrent);
  IMPL_ROLE(FileSystem);
};
}  // namespace

Env& Env::inst() {
  static PosixEnv env;
  return env;
}

}  // namespace cub
