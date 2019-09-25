#include "cub/cli/program_options.h"
#include <sstream>
#include <iostream>

namespace cub {

ProgramOptions::~ProgramOptions() {
  for (auto optset : list) {
    delete optset;
  }
}

void ProgramOptions::add(OptionSet* optset) {
  list.push_back(optset);
}

inline bool ProgramOptions::parseArg(const char* arg) {
  for (auto optset : list) {
    if (optset->parse(arg))
      return true;
  }
  return false;
}

inline bool ProgramOptions::doParse(int argc, const char** argv) {
  for (auto i = 1; i != argc; ++i) {
    if (!parseArg(argv[i])) {
      std::cerr << "invalid option: " << argv[i] << std::endl;
      return false;
    }
  }
  return true;
}

namespace {
inline void head(std::ostream& os, const char* prog) {
  os << prog << '\n' << "option set:\n";
}

inline void body(std::ostream& os, const std::vector<OptionSet*>& list) {
  for (auto opts : list) {
    opts->usage(os);
  }
}
}  // namespace

inline std::string ProgramOptions::usage(const char* prog) const {
  std::ostringstream os("usage: ");
  head(os, prog);
  body(os, list);
  return os.str();
}

bool ProgramOptions::parse(int argc, const char** argv) {
  if (!doParse(argc, argv)) {
    std::cerr << usage(argv[0]) << std::endl;
    return false;
  }
  return true;
}

}  // namespace cub
