// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#ifndef HCF39B75B_BF78_4FA1_9C4D_1BA914C15D81
#define HCF39B75B_BF78_4FA1_9C4D_1BA914C15D81

#include "cub/env/fs/file_system.h"

namespace cub {

struct PosixFileSystem : FileSystem {
private:
  OVERRIDE(ReadOnlyRegion* mmap(const std::string& fname) const);
  OVERRIDE(Status children(const std::string& dir, DirentVisitor& visitor) const);
  OVERRIDE(Status child(const std::string& dir, const std::string& childName, DirentVisitor& visitor) const);
  OVERRIDE(bool exists(const std::string& fname) const);
  OVERRIDE(bool isDir(const std::string& path) const);
  OVERRIDE(Status copyDir(const std::string& from, const std::string& to) const);
  OVERRIDE(Status deleteDir(const std::string& path) const);
};

}  // namespace cub

#endif
