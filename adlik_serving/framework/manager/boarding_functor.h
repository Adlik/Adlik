// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H4DB8EC38_019A_4B0A_AAC2_85A1B093E22D
#define H4DB8EC38_019A_4B0A_AAC2_85A1B093E22D

#include <vector>

namespace adlik {
namespace serving {

struct ModelStream;
struct ManagedStore;

struct BoardingFunctor {
  BoardingFunctor(ManagedStore&);

  void operator()(std::vector<ModelStream>&);

private:
  void flush(std::vector<ModelStream>&);
  void handle(std::vector<ModelStream>&);
  void execute();

  void handleOne(const ModelStream&);
  bool shouldSkip(const ModelStream& stream) const;

private:
  ManagedStore& store;
};

}  // namespace serving
}  // namespace adlik

#endif
