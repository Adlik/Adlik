#ifndef HD7182321_C050_4117_8102_363C30054E93
#define HD7182321_C050_4117_8102_363C30054E93

#include "cub/dci/role.h"

namespace adlik {
namespace serving {

struct ServingStore;
struct ModelFactorySuite;

DEFINE_ROLE(RuntimeContext) {
  HAS_ROLE(ServingStore);
  HAS_ROLE(ModelFactorySuite);
};

}  // namespace serving
}  // namespace adlik

#endif
