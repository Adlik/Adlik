// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H552B9586_FBEF_4CDE_A49D_12E76301EAC3
#define H552B9586_FBEF_4CDE_A49D_12E76301EAC3

#include "cub/cli/option_set.h"

namespace cub {

struct ProgramOptions {
  ~ProgramOptions();

  void add(OptionSet*);
  bool parse(int argc, const char** argv);

private:
  bool doParse(int argc, const char** argv);
  bool parseArg(const char* arg);
  std::string usage(const char* prog) const;

private:
  std::vector<OptionSet*> list;
};

}  // namespace cub

#endif
