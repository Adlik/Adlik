#ifndef CUB_CLI_OPTION_SUBSCRIBER_H
#define CUB_CLI_OPTION_SUBSCRIBER_H

#include "cub/dci/role.h"

namespace cub {

struct ProgramOptions;

DEFINE_ROLE(OptionSubscriber) {
  ABSTRACT(void subscribe(ProgramOptions&));
};

}  // namespace cub

#endif
