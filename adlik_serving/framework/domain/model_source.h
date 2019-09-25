#ifndef H97CA6F4C_A1D7_4430_9F8E_C7883DEC95CA
#define H97CA6F4C_A1D7_4430_9F8E_C7883DEC95CA

#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct ModelTarget;

DEFINE_ROLE(ModelSource) {
  ABSTRACT(void connect(ModelTarget&));
};

}  // namespace serving
}  // namespace adlik

#endif
