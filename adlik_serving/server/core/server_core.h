#ifndef H2CDB23D3_9093_45EC_AE74_60410E54C0C1
#define H2CDB23D3_9093_45EC_AE74_60410E54C0C1

#include "cub/base/status.h"
#include "cub/dci/role.h"

namespace adlik {
namespace serving {

DEFINE_ROLE(ServerCore) {
  static ServerCore& inst();

  ABSTRACT(cub::Status start(int argc, const char** argv));
};

}  // namespace serving
}  // namespace adlik

#endif
