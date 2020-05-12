// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <experimental/filesystem>

#include "cub/base/assertions.h"
#include "cub/env/fs/path.h"
#include "cub/env/posix/posix_filesystem.h"
#include "cub/log/log.h"

namespace sfs = std::experimental::filesystem;

namespace cub {

namespace {
int rmFiles(const char* path, const struct stat* sbuf, int type, struct FTW* ftwb) {
  INFO_LOG << "remove " << path;
  if (remove(path) < 0) {
    INFO_LOG << "remove file error";
    return -1;
  }
  return 0;
}

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
    INFO_LOG << "source dir of " << from << "not exist";
    return cub::NotFound;
  }
  if (exists(to)) {
    INFO_LOG << "target dir of " << to << "is exist";
    return cub::AlreadyExists;
  }
  sfs::copy(from, to, sfs::copy_options::recursive);
  return cub::Success;
}

Status PosixFileSystem::deleteDir(const std::string& name) const {
  INFO_LOG << "delete dir " << name;
  if (!exists(name)) {
    INFO_LOG << "the dir of " << name << "not exist";
  } else {
    nftw(name.c_str(), rmFiles, 10, FTW_DEPTH | FTW_MOUNT | FTW_PHYS);
  }
  return cub::Success;
}

}  // namespace cub
