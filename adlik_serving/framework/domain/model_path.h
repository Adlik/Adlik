// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef H84DA18E2_E058_4B47_93B6_766DF00B4BF7
#define H84DA18E2_E058_4B47_93B6_766DF00B4BF7

#include <string>

namespace adlik {
namespace serving {

struct ModelPath {
  ModelPath(const std::string& base, const std::string& name, int version);
  ModelPath(const std::string& fullPath);

  const std::string& getFullPath() const;

private:
  std::string fullPath;
};

}  // namespace serving
}  // namespace adlik

#endif
