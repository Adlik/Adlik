// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include "cub/env/posix/posix_filesystem.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>

#include "cub/base/assertions.h"
#include "cub/env/fs/path.h"
#include "cub/log/log.h"

namespace cub {

namespace {

struct OperateDir {
  int static copy(const std::string& src, const std::string& dst) {
    srcRoot = normalize(src);
    dstRoot = normalize(dst);
    int status = nftw(src.c_str(), copyFile, 10, FTW_MOUNT | FTW_PHYS);
    srcRoot.clear();
    dstRoot.clear();
    return status;
  }

  int static deleteDir(const std::string& name) {
    return nftw(name.c_str(), rmFile, 10, FTW_DEPTH | FTW_MOUNT | FTW_PHYS);
  }

private:
  static std::string srcRoot;
  static std::string dstRoot;
  static int copyFile(const char* srcPath, const struct stat* sb, int type, struct FTW* ftwbuf) {
    std::string ssrcPath = srcPath;
    std::string dstPath = dstRoot + ssrcPath.substr(srcRoot.size(), ssrcPath.size());
    switch (type) {
      case FTW_D:
        return mkdir(dstPath.c_str(), sb->st_mode);
      case FTW_F:
        std::ifstream src(ssrcPath, std::ios::binary);
        std::ofstream dst(dstPath, std::ios::binary);
        dst << src.rdbuf();
    }
    return 0;
  }
  static int rmFile(const char* path, const struct stat* sbuf, int type, struct FTW* ftwbuf) {
    INFO_LOG << "remove " << path;
    if (remove(path) < 0) {
      INFO_LOG << "remove file error";
      return -1;
    }
    return 0;
  }

  static std::string normalize(std::string path) {
    if (path.back() == '/') {
      return path.substr(0, path.size() - 1);
    } else {
      return path;
    }
  }
};
std::string OperateDir::srcRoot;
std::string OperateDir::dstRoot;

struct PosixReadOnlyRegion : public ReadOnlyRegion {
  PosixReadOnlyRegion(void* addr, uint64_t len) : addr(addr), len(len) {
  }

  ~PosixReadOnlyRegion() override {
    munmap(addr, len);
  }

private:
  OVERRIDE(const void* data() const) {
    return addr;
  }

  OVERRIDE(uint64_t size() const) {
    return len;
  }

private:
  void* addr;
  uint64_t len;
};

struct ReadOnlyRegionMaker {
  ReadOnlyRegionMaker(const std::string& fname) : fd(open(fname.c_str(), O_RDONLY)) {
  }

  ReadOnlyRegion* make() {
    return isValid() ? alloc() : nullptr;
  }

  ~ReadOnlyRegionMaker() {
    if (isValid()) {
      close(fd);
    }
  }

private:
  bool isValid() const {
    return fd >= 0;
  }

  ReadOnlyRegion* alloc() {
    struct stat st;
    ::fstat(fd, &st);
    return alloc(st.st_size);
  }

  ReadOnlyRegion* alloc(size_t size) {
    auto addr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    return addr == MAP_FAILED ? nullptr : new PosixReadOnlyRegion(addr, size);
  }

private:
  int fd;
};
}  // namespace

ReadOnlyRegion* PosixFileSystem::mmap(const std::string& fname) const {
  ReadOnlyRegionMaker maker(fname);
  return maker.make();
}

namespace {
struct DirectReader {
  DirectReader(const std::string& dir) : root(dir), dir(opendir(dir.c_str())) {
  }

  Status read(DirentVisitor& visitor) const {
    CUB_ASSERT_VALID_PTR(dir);
    while (auto entry = readdir(dir)) {
      visitChild(entry->d_name, visitor);
    }
    return cub::Success;
  }

  Status readOne(const std::string& name, DirentVisitor& visitor) const {
    CUB_ASSERT_VALID_PTR(dir);
    visitChild(name, visitor);
    return cub::Success;
  }

  ~DirectReader() {
    if (dir != nullptr) {
      closedir(dir);
    }
  }

private:
  void visitChild(const std::string& baseName, DirentVisitor& visitor) const {
    if (baseName != "." && baseName != "..") {
      visitor.visit(root, baseName);
    }
  }

private:
  std::string root;
  DIR* dir;
};
}  // namespace

Status PosixFileSystem::children(const std::string& dir, DirentVisitor& visitor) const {
  DirectReader reader(dir);
  return reader.read(visitor);
}

Status PosixFileSystem::child(const std::string& dir, const std::string& childName, DirentVisitor& visitor) const {
  DirectReader reader(dir);
  return reader.readOne(childName, visitor);
}

bool PosixFileSystem::exists(const std::string& fname) const {
  return access(fname.c_str(), F_OK) == 0;
}

Status PosixFileSystem::copyDir(const std::string& from, const std::string& to) const {
  INFO_LOG << "copy " << from << " to " << to;
  if (!exists(from)) {
    INFO_LOG << "source dir of " << from << " not exist";
    return cub::NotFound;
  }
  OperateDir::copy(from, to);
  return cub::Success;
}

Status PosixFileSystem::deleteDir(const std::string& name) const {
  INFO_LOG << "delete dir " << name;
  if (!exists(name)) {
    INFO_LOG << "the dir of " << name << " not exist";
  } else {
    OperateDir::deleteDir(name);
  }
  return cub::Success;
}

}  // namespace cub
