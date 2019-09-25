#include "cub/cli/option_set.h"

namespace cub {

OptionSet::OptionSet(std::vector<Option*>&& options)
  : options(std::move(options)) {
}

OptionSet::~OptionSet() {
  for (auto option : options) {
    delete option;
  }
}

bool OptionSet::parse(const char* arg) {
  for (auto option : options) {
    if (option->parse(arg))
      return true;
  }
  return false;
}

void OptionSet::usage(std::ostream& os) const {
  for (auto option : options) {
    os << option->help();
  }
}

}  // namespace cub
