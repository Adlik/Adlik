#ifndef H587CE253_444D_4E03_98D9_EA686D11D883
#define H587CE253_444D_4E03_98D9_EA686D11D883

#include <ostream>
#include <vector>
#include "cub/cli/option.h"

namespace cub {

struct OptionSet {
  OptionSet(std::vector<Option*>&& options);
  ~OptionSet();

  bool parse(const char* arg);
  void usage(std::ostream&) const;

private:
  std::vector<Option*> options;
};

}  // namespace cub

#endif
