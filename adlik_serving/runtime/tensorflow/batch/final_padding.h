#ifndef H1EC38EE4_46BD_4B65_9DDF_9D83E83D5E35
#define H1EC38EE4_46BD_4B65_9DDF_9D83E83D5E35

#include "cub/dci/role.h"

namespace tensorflow {

class Tensor;
struct MergedTensors;

DEFINE_ROLE(FinalPadding) {
  static FinalPadding& nil();

  virtual void fill(const Tensor&, MergedTensors&) const {
  }
};

}  // namespace tensorflow

#endif
